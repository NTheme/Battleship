#include "../lib/network.hpp"

#include "../lib/button.hpp"
#include "../lib/cell.hpp"
#include "../lib/command.hpp"
#include "../lib/game.hpp"
#include "../lib/object.hpp"
#include "../lib/player.hpp"

Network::Network(GameLoop* loop)
    : m_client_thr(&Network::ClientAccept, this),
      m_server_thr(&Network::ServerAccept, this),
      m_loop(loop) {
  m_listener.listen(2000);
}

void Network::Disconnect(bool send) {
  if (send) {
    Send("disconnect");
  }
  m_socket.disconnect();
  m_client_thr.terminate();
  m_server_thr.terminate();
  m_loop->Terminate();
}

bool Network::Connected() { return m_socket.getRemoteAddress() != IpAddress::None; }

size_t Network::GetPort() { return m_listener.getLocalPort(); }

Socket::Status Network::UpdatePort(size_t port) { return m_listener.listen(port); }

void Network::ClientAccept() {
  m_loop->GetWnd().SetShow("client", "status", true, 5);
  auto status = m_socket.connect(m_address.first, m_address.second, sf::milliseconds(1500));
  m_loop->GetWnd().SetShow("client", "status", false, 5);

  switch (status) {
    case Socket::Done:
      m_loop->GetWnd().GetBoxes()["ip"].clear();
      m_loop->GetWnd().SetObject("client", "box", 1, m_loop->GetWnd().GetBoxes()["ip"]);
      m_loop->GetWnd().SetShow("client", "status", true, 0);
      sf::sleep(sf::milliseconds(kMoveSleep));
      m_loop->GetWnd().SetShow("client", "status", false, 0);

      m_loop->LaunchNetwork();

      m_loop->SetLocalPlayer(0);
      m_loop->GetBlocked() = false;
      SetSceneCommand("select_" + bs::atos(m_loop->GetLocalPlayer())).Execute();
      break;
    case Socket::Disconnected:
      m_loop->GetWnd().SetShow("client", "status", true, 2);
      break;
    default:
      m_loop->GetWnd().SetShow("client", "status", true, 3);
  }
}

void Network::ClientConnect(const pair<IpAddress, size_t>& address) {
  m_address = address;
  m_client_thr.launch();
}

void Network::ServerAccept() {
  m_listener.accept(m_socket);
  m_loop->GetWnd().SetShow("client", "status", false, 0);
  m_loop->GetWnd().SetShow("client", "status", true, 1);
  sf::sleep(sf::milliseconds(kMoveSleep));
  m_loop->GetWnd().SetShow("client", "status", false, 1);

  m_loop->LaunchNetwork();
  m_loop->SetLocalPlayer(1);
  m_loop->GetBlocked() = true;
  SetSceneCommand("select_" + bs::atos(m_loop->GetLocalPlayer())).Execute();
}

void Network::ServerConnect() { m_server_thr.launch(); }

void Network::Send(string command_type, string coords) {
  Packet m_packet_out;
  m_packet_out << command_type << coords;
  m_socket.send(m_packet_out);
}

Command* Network::GetCommand() {
  Packet m_packet_in;
  m_socket.receive(m_packet_in);
  string coords;
  string command_type;
  m_packet_in >> command_type >> coords;

  auto ind = std::to_string(1 - m_loop->GetLocalPlayer());
  auto& buttons = m_loop->GetWnd().GetButtons();
  if (command_type == "disconnect") {
    SetSceneCommand("disconnected").Execute();
    return &terminate;
  }
  if (command_type == "add_ship") {
    return buttons.Get("select_" + ind, "ship")->GetCommand().get();
  }
  if (command_type == "add_cell") {
    return buttons.Get("select_" + ind, "cell_m_" + coords)->GetCommand().get();
  }
  return buttons.Get("play_" + ind, "cell_r_" + coords)->GetCommand().get();
}
