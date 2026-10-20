#pragma once
#include "../gfx/canvas.h"
#include "i18n.h"
#include "pvars.h"
#include <string>
#include <utility>
#include <vector>

namespace pui {

struct PrefsSettings {
    std::string root;
    std::string active;
    std::string main;
    int zoom = 0;
    bool onTop = false;
    bool verbose = false;
    std::string language;
    PvarMap pvars;
    bool operator==(const PrefsSettings& o) const {
        return root == o.root && active == o.active && main == o.main && zoom == o.zoom && onTop == o.onTop && verbose == o.verbose && language == o.language &&
               serialize_pvars(pvars) == serialize_pvars(o.pvars);
    }
};

struct PrefsBackend {
    virtual ~PrefsBackend() = default;
    virtual PrefsSettings load() = 0;
    virtual void store(const PrefsSettings& now, const PrefsSettings& before) = 0;
    virtual std::vector<std::string> list_skins(const std::string& root) = 0;
    virtual std::string skin_dir(const std::string& root, const std::string& active) = 0;
    virtual std::string cached_preview(const std::string& skinDir) { (void)skinDir; return {}; }
    virtual std::string diagnostics() { return {}; }
    virtual std::string log_path() { return {}; }
};

namespace prefs_text {
inline constexpr const char* kTabs[] = { N_("General"), N_("Script"), N_("Variables"), N_("Overrides"), N_("Diagnostics") };
inline constexpr const char* kRoot = N_("Skins root folder:");
inline constexpr const char* kChooseFolder = N_("Choose a skin folder, or a folder with one skin per subfolder");
inline constexpr const char* kSkin = N_("Active skin:");
inline constexpr const char* kMain = N_("Main script:");
inline constexpr const char* kZoom = N_("Zoom:");
inline constexpr const char* kLanguage = N_("Language:");
inline constexpr const char* kLanguageNote = N_("Restart foobar2000 to apply a new language.");
inline constexpr const char* kZoomAutoScaled = N_("Automatic (display scaling)");
inline constexpr const char* kZoomAuto100 = N_("Automatic (100%)");
inline constexpr const char* kOnTop = N_("Keep the player window on top of other windows");
inline constexpr const char* kWizard = N_("Layout wizard...");
inline constexpr const char* kWizardNote = N_("Start again from a ready-made layout.");
inline constexpr const char* kWizardUnavailable = N_("The layout wizard runs in the Panels UI player window, which isn't open: "
                                                  "pick Panels UI as the user interface first.");
inline constexpr const char* kOwnFolder = N_("(the component's own folder)");
inline constexpr const char* kAutomatic = N_("(automatic)");
inline constexpr const char* kNoPreview = N_("No preview yet: a skin gets one the first time it is shown.");
inline constexpr const char* kScript = N_("Active skin's main script:");
inline constexpr const char* kVarsNote = N_("Persistent variables ($getpvar/$setpvar) the skin uses. Double-click a value to edit it.");
inline constexpr const char* kVarsFind = N_("Find the skin's variables");
inline constexpr const char* kVariable = N_("Variable");
inline constexpr const char* kValue = N_("Value");
inline constexpr const char* kOverridesNote = N_("Fallbacks used when the active skin doesn't set its own font / accent colour. "
                                              "Leave blank to defer to the skin.");
inline constexpr const char* kFontFace = N_("Font face:");
inline constexpr const char* kFontSize = N_("Size:");
inline constexpr const char* kAccent = N_("Accent colour:");
inline constexpr const char* kClearOverrides = N_("Clear overrides");
inline constexpr const char* kDiagNote = N_("Found a bug? Turn on verbose logging, make the problem happen again, then "
                                         "copy the diagnostics and paste them into a new GitHub issue. Paths have your "
                                         "user name replaced; nothing is sent anywhere by itself.");
inline constexpr const char* kVerbose = N_("Verbose logging (for bug reports)");
inline constexpr const char* kVerboseNote = N_("Without it, only warnings and errors go to the log file.");
inline constexpr const char* kCopyDiagnostics = N_("Copy Diagnostics");
inline constexpr const char* kCopied = N_("Copied to the clipboard. Paste it into the bug report.");
inline constexpr const char* kCopyFailed = N_("Couldn't copy the diagnostics to the clipboard.");
inline constexpr const char* kOpenLogFolder = N_("Open Log Folder");
inline constexpr const char* kShowLog = N_("Show Log in Finder");
inline constexpr const char* kReportIssue = N_("Report an Issue...");
inline constexpr const char* kLogFile = N_("Log file:");
inline constexpr const char* kNoLog = N_("(not open yet)");
inline constexpr const char* kBrowse = N_("Browse...");
inline constexpr const char* kChoose = N_("Choose…");
inline constexpr const char* kClear = N_("Clear");
inline constexpr const char* kNoRoot = N_("(none: the component's own folder)");
inline constexpr const char* kSave = N_("Save");
inline constexpr const char* kRevert = N_("Revert");
inline constexpr const char* kPickFont = N_("Pick font...");
inline constexpr const char* kPickColour = N_("Pick colour...");
inline constexpr const char* kUseSkinAccent = N_("Use the skin's");
inline constexpr const char* kSkinDefault = N_("the skin's");
inline constexpr const char* kAuthor = N_("Author: Santiago Rodriguez");
inline constexpr const char* kSourceUrl = "https://github.com/santiagorod92/foo_ui_panels";
}

class PrefsModel {
public:
    static constexpr const char* kFontFaceKey = "_prefs_font_face";
    static constexpr const char* kFontSizeKey = "_prefs_font_size";
    static constexpr const char* kAccentKey = "_prefs_accent_color";
    static bool is_reserved(const std::string& key) { return key.rfind("_prefs_", 0) == 0; }

    explicit PrefsModel(PrefsBackend& b) : m_b(b) {}

    void load();
    bool changed() const;
    void apply();
    void reset();
    const PrefsSettings& pending() const { return m_now; }

    const std::vector<std::string>& skins() const { return m_skins; }
    void set_root(const std::string& root);
    void set_active(const std::string& skin);
    void set_main(const std::string& file);
    void choose_skin_folder(const std::string& folder);
    std::string chosen_folder_problem(const std::string& folder) const;
    std::string skin_dir() const;
    std::vector<std::string> main_choices() const;
    std::string main_script(std::string* why = nullptr) const;
    std::string skin_warning() const;
    std::string root_note() const;

    std::vector<std::string> skin_labels() const;
    int skin_index() const;
    void set_skin_index(int i);
    std::vector<std::string> main_labels() const;
    int main_index() const;
    void set_main_index(int i);
    std::string preview_image() const;
    static std::string skin_preview_file(const std::string& skinDir);

    const std::string& script() const { return m_script; }
    const std::string& script_path() const { return m_scriptPath; }
    void set_script(const std::string& text) { m_script = text; }

    static const std::vector<int>& zoom_choices();
    static std::vector<std::string> zoom_labels(const std::string& automatic);
    int zoom_index() const;
    void set_zoom_index(int i);
    void set_on_top(bool on) { m_now.onTop = on; }
    void set_verbose(bool on) { m_now.verbose = on; }

    static std::vector<std::string> language_labels();
    int language_index() const;
    void set_language_index(int i);

    std::string diagnostics() const { return m_b.diagnostics(); }
    std::string log_path() const { return m_b.log_path(); }

    std::string font_face() const { return get(kFontFaceKey); }
    std::string font_size() const { return get(kFontSizeKey); }
    void set_font(const std::string& face, const std::string& size);
    bool accent(gfx::Color& out) const;
    void set_accent(gfx::Color c);
    void clear_accent();
    void clear_overrides();

    std::vector<std::pair<std::string, std::string>> variables() const;
    void set_variable(const std::string& key, const std::string& value);
    void rescan_variables();
    static std::vector<std::string> scan_pvar_names(const std::string& script);

private:
    std::string get(const char* k) const { auto it = m_now.pvars.find(k); return it == m_now.pvars.end() ? std::string() : it->second; }
    void refresh_skins();
    void load_script();

    PrefsBackend& m_b;
    PrefsSettings m_now, m_applied;
    std::vector<std::string> m_skins;
    std::string m_script, m_appliedScript, m_scriptPath;
};

}
