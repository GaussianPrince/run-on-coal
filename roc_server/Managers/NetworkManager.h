#pragma once
#define ROC_NETWORK_MAX_CONNECTIONS 8
#define ROC_NETWORK_DISCONNECT_DURATION 300U

namespace ROC
{

class Core;
class Client;
class LuaArguments;
class Pool;
class NetworkManager
{
    Core *m_core;

    RakNet::RakPeerInterface *m_networkInterface;
    enum NetworkIdentifier : unsigned char { ID_ROC_DATA_PACKET = ID_USER_PACKET_ENUM + 1 };

    std::unordered_map<RakNet::SystemIndex, Client*> m_clientMap;
    std::unordered_map<RakNet::SystemIndex, Client*>::iterator m_clientMapEnd;

    LuaArguments *m_argument;
public:
    bool Disconnect(Client *f_client);
    bool SendData(Client *f_client, std::string &f_data);
protected:
    explicit NetworkManager(Core *f_core);
    ~NetworkManager();

    void DoPulse();
    friend Core;
};

}