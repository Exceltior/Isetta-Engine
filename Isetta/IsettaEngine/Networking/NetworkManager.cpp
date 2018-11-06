/*
 * Copyright (c) 2018 Isetta
 */

#include "Networking/NetworkManager.h"
#include "Core/Config/Config.h"
#include "Networking/NetworkId.h"
#include "Networking/NetworkingModule.h"
#include "Components/NetworkTransform.h"
#include "Scene/Entity.h"

namespace Isetta {

NetworkManager& NetworkManager::Instance() {
  static NetworkManager instance;
  return instance;
}

void NetworkManager::SendMessageFromClient(yojimbo::Message* message) {
  networkingModule->AddClientToServerMessage(message);
}

void NetworkManager::SendMessageFromServer(int clientIdx,
                                           yojimbo::Message* message) {
  networkingModule->AddServerToClientMessage(clientIdx, message);
}

void NetworkManager::ConnectToServer(const char* serverAddress,
                                     Action<bool> callback) {
  networkingModule->Connect(
      serverAddress, Config::Instance().networkConfig.serverPort.GetVal(),
      callback);
}

void NetworkManager::DisconnectFromServer() { networkingModule->Disconnect(); }

void NetworkManager::CreateServer(const char* address) {
  networkingModule->CreateServer(
      address, Config::Instance().networkConfig.serverPort.GetVal());
}

bool NetworkManager::LocalClientIsConnected() {
  return networkingModule->client->IsConnected();
}

bool NetworkManager::ClientIsConnected(int clientIdx) {
  return networkingModule->server->IsClientConnected(clientIdx);
}

bool NetworkManager::ServerIsRunning() {
  return networkingModule->server && networkingModule->server->IsRunning();
}

int NetworkManager::GetMaxClients() {
  return Config::Instance().networkConfig.maxClients.GetVal();
}

int NetworkManager::GetClientIndex() {
  return networkingModule->client->GetClientIndex();
}

void NetworkManager::CloseServer() { networkingModule->CloseServer(); }

std::list<std::pair<U16, Action<yojimbo::Message*>>>
NetworkManager::GetClientFunctions(int type) {
  return clientCallbacks[type];
}
std::list<std::pair<U16, Action<int, yojimbo::Message*>>>
NetworkManager::GetServerFunctions(int type) {
  return serverCallbacks[type];
}

yojimbo::Message* NetworkManager::CreateClientMessage(int messageId) {
  return networkingModule->client->CreateMessage(messageId);
}

yojimbo::Message* NetworkManager::CreateServerMessage(int clientIdx,
  int messageId) {
  return networkingModule->server->CreateMessage(clientIdx, messageId);
}

Entity* NetworkManager::GetNetworkEntity(const U32 id) {
  auto it = networkIdToComponentMap.find(id);
  if (it != networkIdToComponentMap.end()) {
    return it->second->entity;
  }
  return NULL;
}

NetworkId* NetworkManager::GetNetworkId(const U32 id) {
  auto it = networkIdToComponentMap.find(id);
  if (it != networkIdToComponentMap.end()) {
    return it->second;
  }
  return NULL;
}

U32 NetworkManager::CreateNetworkId(NetworkId* NetworkId) {
  if (!ServerIsRunning()) {
    throw std::exception("Cannot create a new network id on a client");
  }
  U32 netId = nextNetworkId++;
  NetworkId->id = netId;
  networkIdToComponentMap[netId] = NetworkId;
}

U32 NetworkManager::AssignNetworkId(U32 netId,
                                    NetworkId* NetworkId) {
  if (networkIdToComponentMap.find(netId) != networkIdToComponentMap.end()) {
    throw std::exception(Util::StrFormat(
        "Multiple objects trying to assign to the same network id: %d", netId));
  } else if (NetworkId->id > 0) {
    throw std::exception(
        Util::StrFormat("Trying to assign network id %d to existing network "
                        "object with id %d",
                        netId, NetworkId->id));
  }
  NetworkId->id = netId;
  networkIdToComponentMap[netId] = NetworkId;
}

void NetworkManager::RemoveNetworkId(NetworkId* NetworkId) {
  if (!NetworkId->id) {
    throw std::exception(Util::StrFormat(
        "Cannot remove network id on a nonexistent network object"));
  }
  networkIdToComponentMap.erase(NetworkId->id);
  NetworkId->id = NULL;
}

U32 NetworkManager::CreateNetId() {
  return nextNetworkId++;
}
}  // namespace Isetta