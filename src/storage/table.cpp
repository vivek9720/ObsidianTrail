#include "obsidiantrail/storage/table.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::storage {

static std::vector<std::string> parse_row(const std::string& line, char delimiter) {
    std::vector<std::string> out;
    std::string cell;
    bool quote = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (ch == '"') quote = !quote;
        else if (ch == delimiter && !quote) { out.push_back(cell); cell.clear(); }
        else cell.push_back(ch);
    }
    out.push_back(cell);
    for (auto& item : out) item = core::trim(item);
    return out;
}

Table parse_delimited_table(const std::string& text, char delimiter) {
    Table table;
    bool header = true;
    for (const auto& line : core::lines(text)) {
        if (core::trim(line).empty()) continue;
        auto cells = parse_row(line, delimiter);
        if (header) {
            for (auto& cell : cells) table.columns.push_back(core::normalize_key(cell));
            header = false;
            continue;
        }
        Row row;
        for (std::size_t i = 0; i < cells.size() && i < table.columns.size(); ++i) row.values[table.columns[i]] = cells[i];
        table.rows.push_back(std::move(row));
    }
    return table;
}

std::string render_delimited_table(const Table& table, char delimiter) {
    std::ostringstream out;
    for (std::size_t i = 0; i < table.columns.size(); ++i) {
        if (i) out << delimiter;
        out << table.columns[i];
    }
    out << "\n";
    for (const auto& row : table.rows) {
        for (std::size_t i = 0; i < table.columns.size(); ++i) {
            if (i) out << delimiter;
            auto it = row.values.find(table.columns[i]);
            if (it != row.values.end()) out << it->second;
        }
        out << "\n";
    }
    return out.str();
}

Table project(const Table& table, const std::vector<std::string>& columns) {
    Table out;
    for (const auto& column : columns) out.columns.push_back(core::normalize_key(column));
    for (const auto& row : table.rows) {
        Row projected;
        for (const auto& column : out.columns) {
            auto it = row.values.find(column);
            if (it != row.values.end()) projected.values[column] = it->second;
        }
        out.rows.push_back(std::move(projected));
    }
    return out;
}

Table filter_equals(const Table& table, const std::string& column, const std::string& value) {
    Table out;
    out.columns = table.columns;
    auto key = core::normalize_key(column);
    for (const auto& row : table.rows) {
        auto it = row.values.find(key);
        if (it != row.values.end() && core::lower(it->second) == core::lower(value)) out.rows.push_back(row);
    }
    return out;
}

std::map<std::string, std::size_t> histogram(const Table& table, const std::string& column) {
    std::map<std::string, std::size_t> counts;
    auto key = core::normalize_key(column);
    for (const auto& row : table.rows) {
        auto it = row.values.find(key);
        counts[it == row.values.end() ? "" : it->second]++;
    }
    return counts;
}

std::string summarize_table(const Table& table) {
    std::ostringstream out;
    out << "columns=" << table.columns.size() << " rows=" << table.rows.size();
    for (const auto& column : table.columns) out << "\ncolumn: " << column;
    return out.str();
}

} // namespace obsidiantrail::storage
