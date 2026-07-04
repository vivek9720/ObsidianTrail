#ifndef OBSIDIANTRAIL_STORAGE_TABLE_HPP
#define OBSIDIANTRAIL_STORAGE_TABLE_HPP

#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::storage {

struct Row {
    std::map<std::string, std::string> values;
};

struct Table {
    std::vector<std::string> columns;
    std::vector<Row> rows;
};

Table parse_delimited_table(const std::string& text, char delimiter = ',');
std::string render_delimited_table(const Table& table, char delimiter = ',');
Table project(const Table& table, const std::vector<std::string>& columns);
Table filter_equals(const Table& table, const std::string& column, const std::string& value);
std::map<std::string, std::size_t> histogram(const Table& table, const std::string& column);
std::string summarize_table(const Table& table);

} // namespace obsidiantrail::storage

#endif
