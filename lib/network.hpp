#pragma once

#include "command.hpp"
#include "common.hpp"

class Network {
public:
  explicit Network(GameLoop* loop);

  void Disconnect(bool send = true);
  bool Connected() const;
  size_t GetPort() const;
  Socket::Status UpdatePort(size_t port);

  void ClientAccept();
  void ClientConnect(const std::pair<IpAddress, size_t>& address);
  void ServerAccept();
  void ServerConnect();

  void Send(const std::string& command_type, const std::string& coords = "");
  Command* GetCommand();

private:
  std::pair<IpAddress, size_t> m_address{IpAddress::Any, 0};
  TcpSocket m_socket;
  TcpListener m_listener;

  std::thread m_client_thr;
  std::thread m_server_thr;
  std::atomic<bool> m_clientRunning{false};
  std::atomic<bool> m_serverRunning{false};

  GameLoop* m_loop;
  DisconnectCommand terminate;
};