#pragma once
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace pui::i18n {

enum class Language { English, Spanish };

using Resolver = Language (*)();

Language parse_language_code(const std::string& code);
const char* language_code(Language lang);
Language resolve_language(const std::string& setting, const std::string& systemCode);

void set_resolver(Resolver resolver);
void set_language(Language lang);
Language language();

const char* tr(const char* english);
std::string tr(const std::string& english);
std::string trf(const char* english, std::initializer_list<std::string> args);
std::string substitute(const std::string& pattern, std::initializer_list<std::string> args);

const std::vector<std::pair<const char*, const char*>>& spanish_table();

struct LanguageOption { const char* label; const char* value; };

const std::vector<LanguageOption>& language_options();
int language_option_index(const std::string& value);

}

namespace pui {
constexpr const char* N_(const char* english) { return english; }
using i18n::tr;
using i18n::trf;
}
