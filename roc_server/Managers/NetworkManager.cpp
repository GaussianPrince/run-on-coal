#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/ConfigManager.h"
#include "Managers/EventManager.h"
#include "Managers/ElementManager.h"
#include "Managers/LogManager.h"
#include "Managers/LuaManager.h"
#include "Managers/NetworkManager.h"
#include "Elements/Client.h"
#include "Lua/LuaArguments.h"

unsigned char GetPacketIdentifier(RakNet::Packet *f_packet)
{
    unsigned char l_result = 255U;
    if(f_packet)
    {
        if(f_packet->data[0] == ID_TIMESTAMP)
        {
            RakAssert(f_packet->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
            l_result = f_packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
        }
        else l_result = f_packet->data[0];
    }
    return l_result;
}

ROC::NetworkManager::NetworkManager(Core *f_core)
{
    m_core = f_core;

    m_networkInterface = RakNet::RakPeerInterface::GetInstance();
    std::string l_string;
    m_core->GetConfigManager()->GetBindIP(l_string);
    RakNet::SocketDescriptor l_socketDescriptor(m_core->GetConfigManager()->GetBindPort(), l_string.c_str());
    if(m_networkInterface->Startup(m_core->GetConfigManager()->GetMaxClients(), &l_socketDescriptor, 1) == RakNet::StartupResult::RAKNET_STARTED)
    {
        std::string l_log("Network interface started at ");
        l_log.append(l_socketDescriptor.hostAddress);
        l_log.push_back(':');
        l_log.append(std::to_string(l_socketDescriptor.port));
        m_core->GetLogManager()->Log(l_log);
        l_log.clear();
        l_log.append("Maximum clients - ");
        l_log.append(std::to_string(m_core->GetConfigManager()->GetMaxClients()));
        m_core->GetLogManager()->Log(l_log);

        m_networkInterface->SetMaximumIncomingConnections(m_core->GetConfigManager()->GetMaxClients());
    }
    else
    {
        m_core->GetLogManager()->Log("Unable to start network interface. Check IP and port in configuration file");
        RakNet::RakPeerInterface::DestroyInstance(m_networkInterface);
        m_networkInterface = NULL;
    }

    m_clientMap = std::unordered_map<RakNet::SystemIndex, Client*>();
    m_clientMapEnd = m_clientMap.end();
    m_argument = new LuaArguments();
}
ROC::NetworkManager::~NetworkManager()
{
    if(m_networkInterface)
    {
        m_networkInterface->Shutdown(ROC_NETWORK_DISCONNECT_DURATION);
        RakNet::RakPeerInterface::DestroyInstance(m_networkInterface);
    }
    delete m_argument;
}

bool ROC::NetworkManager::Disconnect(Client *f_client)
{
    if(m_networkInterface) m_networkInterface->CloseConnection(f_client->GetAddress(), true);
    return (m_networkInterface != NULL);
}
bool ROC::NetworkManager::SendData(Client *f_client, std::string &f_data)
{
    if(m_networkInterface)
    {
        RakNet::BitStream l_sendData;
        unsigned int l_dataSize = static_cast<unsigned int>(f_data.size());
        l_sendData.Write(static_cast<unsigned char>(ID_ROC_DATA_PACKET));
        l_sendData.Write(l_dataSize);
        l_sendData.Write(f_data.data(),l_dataSize);
        m_networkInterface->Send(&l_sendData, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, f_client->GetAddress(), false);
    }
    return (m_networkInterface != NULL);
}

void ROC::NetworkManager::DoPulse()
{
    if(m_networkInterface)
    {
        RakNet::Packet* l_packet;
        for(l_packet = m_networkInterface->Receive(); l_packet; m_networkInterface->DeallocatePacket(l_packet), l_packet = m_networkInterface->Receive())
        {
            switch(GetPacketIdentifier(l_packet))
            {
                case ID_NEW_INCOMING_CONNECTION:
                {
                    std::string l_log("New client connected (");

                    Client *l_client = m_core->GetElementManager()->CreateClient();
                    l_client->SetAddress(l_packet->systemAddress);
                    m_clientMap.insert(std::pair<RakNet::SystemIndex, Client*>(l_packet->guid.systemIndex, l_client));
                    m_clientMapEnd = m_clientMap.end();

                    m_argument->PushArgument(reinterpret_cast<void*>(l_client));
                    m_core->GetLuaManager()->GetEventManager()->CallEvent(EventType::NetworkClientConnect, m_argument);
                    m_argument->Clear();

                    l_log.append(l_packet->systemAddress.ToString(true, ':'));
                    l_log.append(") with ID ");
                    l_log.append(std::to_string(l_packet->guid.systemIndex));
                    m_core->GetLogManager()->Log(l_log);
                } break;
                case ID_DISCONNECTION_NOTIFICATION: case ID_CONNECTION_LOST:
                {
                    auto iter = m_clientMap.find(l_packet->guid.systemIndex);
                    if(iter != m_clientMapEnd)
                    {
                        Client *l_client = (*iter).second;
                        std::string l_log("Client (");
                        l_log.append(l_packet->systemAddress.ToString(true, ':'));
                        l_log.append(") with ID ");
                        l_log.append(std::to_string(l_client->GetID()));
                        l_log.append(" disconnected");

                        m_argument->PushArgument(reinterpret_cast<void*>(l_client));
                        m_core->GetLuaManager()->GetEventManager()->CallEvent(EventType::NetworkClientDisconnect, m_argument);
                        m_argument->Clear();

                        m_core->GetElementManager()->DestroyClient(l_client);
                        m_clientMap.erase(iter);
                        m_clientMapEnd = m_clientMap.end();

                        m_core->GetLogManager()->Log(l_log);
                    }
                } break;
                case ID_ROC_DATA_PACKET:
                {
                    auto iter = m_clientMap.find(l_packet->guid.systemIndex);
                    if(iter != m_clientMapEnd)
                    {
                        RakNet::BitStream l_dataIn(l_packet->data, l_packet->length, false);
                        unsigned int l_textSize;
                        std::string l_stringData;
                        l_dataIn.IgnoreBytes(sizeof(unsigned char));
                        l_dataIn.Read(l_textSize);
                        l_stringData.resize(l_textSize);
                        l_dataIn.Read(const_cast<char*>(l_stringData.data()), l_textSize);

                        m_argument->PushArgument(reinterpret_cast<void*>((*iter).second));
                        m_argument->PushArgument(&l_stringData);
                        m_core->GetLuaManager()->GetEventManager()->CallEvent(EventType::NetworkDataRecieve, m_argument);
                        m_argument->Clear();
                    }
                } break;
            }
        }
    }
}
