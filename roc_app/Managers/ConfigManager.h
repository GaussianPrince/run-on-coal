#pragma once

namespace ROC
{

class ConfigManager final
{
    bool m_logging;
    bool m_fullscreen;
    int m_antialiasing;
    glm::ivec2 m_windowSize;
    unsigned int m_fpsLimit;
    bool m_vsync;
    bool m_vrMode;
    std::vector<std::string> m_modules;

    ConfigManager(const ConfigManager &that) = delete;
    ConfigManager& operator=(const ConfigManager &that) = delete;
public:
    inline bool IsLogEnabled() const { return m_logging; }
    inline bool IsFullscreenEnabled() const { return m_fullscreen; }
    inline int GetAntialiasing() const { return m_antialiasing; }
    inline const glm::ivec2& GetWindowSize() const { return m_windowSize; }
    inline unsigned int GetFPSLimit() const { return m_fpsLimit; }
    inline bool GetVSync() const { return m_vsync; }
    inline bool IsVRModeEnabled() const { return m_vrMode; }
    inline const std::vector<std::string>& GetModules() const { return m_modules; }
protected:
    ConfigManager();
    ~ConfigManager();

    friend class Core;
};

}
