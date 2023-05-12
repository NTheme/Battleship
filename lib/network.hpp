#pragma once

#include "command.hpp"
#include "common.hpp"

class Network {
 public:
  explicit Network(GameLoop* loop);
  void Disconnect(bool send = true);
  bool Connected();
  size_t GetPort();
  Socket::Status UpdatePort(size_t port);
  void ClientAccept();
  void ClientConnect(const pair<IpAddress, size_t>& address);
  void ServerAccept();
  void ServerConnect();

  void Send(string command_type, string coords = "");
  Command* GetCommand();

 private:
  pair<IpAddress, size_t> m_address;
  TcpSocket m_socket;
  TcpListener m_listener;
  Thread m_client_thr;
  Thread m_server_thr;
  GameLoop* m_loop;
  DisconnectCommand terminate;
};
