#ifndef OBSIDIANTRAIL_ANALYSIS_NORMALIZATION_CATALOG_HPP
#define OBSIDIANTRAIL_ANALYSIS_NORMALIZATION_CATALOG_HPP

#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct FieldAlias {
    std::string source;
    std::string original;
    std::string canonical;
    std::string type;
    std::string treatment;
    std::string note;
};

struct ParserRecipe {
    std::string source;
    std::string format;
    std::string record_boundary;
    std::string timestamp_field;
    std::string identity_field;
    std::string object_field;
    std::string caveat;
};

const std::vector<FieldAlias>& field_aliases();
const std::vector<ParserRecipe>& parser_recipes();
std::vector<FieldAlias> aliases_for_source(const std::string& source);
std::vector<FieldAlias> aliases_for_canonical(const std::string& canonical);
std::vector<ParserRecipe> recipes_for_format(const std::string& format);
std::string normalization_catalog_summary();

} // namespace obsidiantrail::analysis

#endif
