#include "../lib/network.hpp"
#include "../lib/button.hpp"
#include "../lib/command.hpp"
#include "../lib/game.hpp"

Network::Network(GameLoop* loop)
  : m_loop(loop) {
  std::ignore = m_listener.listen(2000);
}

void Network::Disconnect(const bool send) {
  if (send)
    Send("disconnect");
  m_socket.disconnect();

  m_clientRunning = false;
  m_serverRunning = false;
  m_listener.close();

  if (m_client_thr.joinable()
      && std::this_thread::get_id() != m_client_thr.get_id())
    m_client_thr.join();

  if (m_server_thr.joinable()
      && std::this_thread::get_id() != m_server_thr.get_id())
    m_server_thr.join();

  m_loop->GetWnd().Post([loop = m_loop] {
    loop->Terminate();
  });
}

bool Network::Connected() const {
  return m_socket.getRemoteAddress().has_value();
}

size_t Network::GetPort() const {
  return m_listener.getLocalPort();
}

Socket::Status Network::UpdatePort(const size_t port) {
  return m_listener.listen(static_cast<unsigned short>(port));
}

void Network::ClientAccept() {
  m_clientRunning = true;

  m_loop->GetWnd().Post([this] {
    m_loop->GetWnd().SetShow("client", "status", true, 5);
  });

  const auto st = m_socket.connect(
      m_address.first,
      static_cast<unsigned short>(m_address.second),
      sf::milliseconds(1500)
      );

  if (st == Socket::Status::Done) {
    m_loop->GetWnd().Post([this] {
      auto& wnd = m_loop->GetWnd();
      wnd.SetShow("client", "status", false, 5);
      wnd.GetBoxes()["ip"].clear();
      wnd.SetObject("client", "box", 1, wnd.GetBoxes()["ip"]);
      wnd.SetShow("client", "status", true, 0);
    });

    sf::sleep(sf::milliseconds(kMoveSleep));

    m_loop->GetWnd().Post([this] {
      m_loop->GetWnd().SetShow("client", "status", false, 0);
      m_loop->LaunchNetwork();
      m_loop->SetLocalPlayer(0);
      m_loop->GetBlocked() = false;
      SetSceneCommand("select_" + bs::atos(m_loop->GetLocalPlayer())).Execute();
    });
  } else {
    m_loop->GetWnd().Post([this, st] {
      m_loop->GetWnd().SetShow("client", "status", false,
                               5);
      m_loop->GetWnd().SetShow("client", "status", true,
                               st == Socket::Status::Disconnected ? 2 : 3);
    });
  }

  m_clientRunning = false;
}

void Network::ClientConnect(const std::pair<IpAddress, size_t>& address) {
  m_address = address;
  if (m_client_thr.joinable())
    m_client_thr.join();
  m_client_thr = std::thread(&Network::ClientAccept, this);
}

void Network::ServerAccept() {
  while (m_serverRunning) {
    const auto st = m_listener.accept(m_socket);
    if (st == Socket::Status::NotReady) {
      sf::sleep(sf::milliseconds(10));
      continue;
    }
    if (st != Socket::Status::Done) {
      break;
    }

    m_loop->GetWnd().Post([this] {
      m_loop->GetWnd().SetShow("client", "status", false, 0);
      m_loop->GetWnd().SetShow("client", "status", true, 1);
    });

    sf::sleep(sf::milliseconds(kMoveSleep));

    m_loop->GetWnd().Post([this] {
      m_loop->GetWnd().SetShow("client", "status", false, 1);
      m_loop->LaunchNetwork();
      m_loop->SetLocalPlayer(1);
      m_loop->GetBlocked() = true;
      SetSceneCommand("select_" + bs::atos(m_loop->GetLocalPlayer())).Execute();
    });
    break;
  }
}

void Network::ServerConnect() {
  if (m_server_thr.joinable())
    m_server_thr.join();
  m_listener.setBlocking(false);
  m_serverRunning = true;
  m_server_thr = std::thread(&Network::ServerAccept, this);
}

void Network::Send(const std::string& command_type, const std::string& coords) {
  Packet p;
  p << command_type << coords;
  (void)m_socket.send(p);
}

Command* Network::GetCommand() {
  Packet p;
  if (const auto res = m_socket.receive(p); res != Socket::Status::Done) {
    SetSceneCommand("disconnected").Execute();
    return &terminate;
  }

  std::string coords, command_type;
  p >> command_type >> coords;

  const auto ind = std::to_string(1 - m_loop->GetLocalPlayer());
  auto& buttons = m_loop->GetWnd().GetButtons();

  if (command_type == "disconnect") {
    SetSceneCommand("disconnected").Execute();
    return &terminate;
  }
  if (command_type == "add_ship")
    return buttons.Get("select_" + ind, "ship")->GetCommand().get();
  if (command_type == "add_cell")
    return buttons.Get("select_" + ind, "cell_m_" + coords)->GetCommand().get();
  return buttons.Get("play_" + ind, "cell_r_" + coords)->GetCommand().get();
}
