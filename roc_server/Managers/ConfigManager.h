#pragma once

namespace ROC
{

class ConfigManager final
{
    bool m_configParsed;
    bool m_logging;
    std::string m_bindIP;
    unsigned short m_bindPort;
    unsigned short m_maxClients;
    unsigned int m_pulseTick;
    std::vector<std::string> m_modules;

    ConfigManager(const ConfigManager &that) = delete;
    ConfigManager& operator=(const ConfigManager &that) = delete;
public:
    inline bool IsConfigParsed() const { return m_configParsed; }
    inline bool IsLogEnabled() const { return m_logging; }
    inline void GetBindIP(std::string &f_ip) const { f_ip.assign(m_bindIP); }
    inline unsigned short GetBindPort() const { return m_bindPort; }
    inline unsigned short GetMaxClients() const { return m_maxClients; }
    inline unsigned int GetPulseTick() const { return m_pulseTick; }
    inline const std::vector<std::string>& GetModules() const { return m_modules; }
protected:
    ConfigManager();
    ~ConfigManager();
    friend class Core;
};

}
