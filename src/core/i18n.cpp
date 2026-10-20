#include "i18n.h"
#include <atomic>
#include <cctype>
#include <string_view>
#include <unordered_map>

namespace pui::i18n {

namespace {

constexpr int kUnresolved = -1;

std::atomic<int> g_language{ kUnresolved };
std::atomic<Resolver> g_resolver{ nullptr };

const std::unordered_map<std::string_view, const char*>& spanish_map() {
    static const auto map = [] {
        std::unordered_map<std::string_view, const char*> m;
        for (const auto& [en, es] : spanish_table()) m.emplace(en, es);
        return m;
    }();
    return map;
}

}

Language parse_language_code(const std::string& code) {
    if (code.size() >= 2 && std::tolower((unsigned char)code[0]) == 'e' && std::tolower((unsigned char)code[1]) == 's' &&
        (code.size() == 2 || code[2] == '-' || code[2] == '_' || code[2] == '.'))
        return Language::Spanish;
    return Language::English;
}

const char* language_code(Language lang) { return lang == Language::Spanish ? "es" : "en"; }

Language resolve_language(const std::string& setting, const std::string& systemCode) {
    if (setting == "en") return Language::English;
    if (setting == "es") return Language::Spanish;
    return parse_language_code(systemCode);
}

void set_resolver(Resolver resolver) { g_resolver.store(resolver); }

void set_language(Language lang) { g_language.store((int)lang); }

Language language() {
    int lang = g_language.load();
    if (lang == kUnresolved) {
        const Resolver r = g_resolver.load();
        lang = (int)(r ? r() : Language::English);
        g_language.store(lang);
    }
    return (Language)lang;
}

const char* tr(const char* english) {
    if (!english || language() == Language::English) return english;
    const auto& map = spanish_map();
    const auto it = map.find(english);
    return it == map.end() ? english : it->second;
}

std::string tr(const std::string& english) { return tr(english.c_str()); }

std::string substitute(const std::string& pattern, std::initializer_list<std::string> args) {
    std::string out;
    out.reserve(pattern.size());
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '{' && i + 2 < pattern.size() && pattern[i + 2] == '}' && std::isdigit((unsigned char)pattern[i + 1])) {
            const size_t n = (size_t)(pattern[i + 1] - '0');
            if (n < args.size()) {
                out += *(args.begin() + n);
                i += 2;
                continue;
            }
        }
        out.push_back(pattern[i]);
    }
    return out;
}

std::string trf(const char* english, std::initializer_list<std::string> args) { return substitute(tr(english), args); }

const std::vector<LanguageOption>& language_options() {
    static const std::vector<LanguageOption> k = {
        { N_("Automatic (system language)"), "" },
        { "English", "en" },
        { "Español", "es" },
    };
    return k;
}

int language_option_index(const std::string& value) {
    const auto& opts = language_options();
    for (size_t i = 0; i < opts.size(); ++i)
        if (value == opts[i].value) return (int)i;
    return 0;
}

}
