#include "../lib/command.hpp"

#include <iostream>

#include "../lib/button.hpp"
#include "../lib/cell.hpp"
#include "../lib/game.hpp"
#include "../lib/player.hpp"
#include "../lib/window.hpp"

GameLoop* Command::m_loop = nullptr;

Command::Command() : m_type(MyEventType::Count) {
}

Command::Command(MyEventType type) : m_type(type) {
}

MyEventType Command::GetType() const noexcept {
  return m_type;
}

void IPBoxCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetShow("client", "status", false);

  if (const size_t code = m_loop->GetWnd().GetEvent().getIf<Event::TextEntered>()->unicode;
    code == 8 && !m_loop->GetWnd().GetBoxes()["ip"].empty()) {
    m_loop->GetWnd().GetBoxes()["ip"].pop_back();
  } else if (code == 13) {
    IPClientCommand().Execute();
  } else if (code >= 46 && code <= 58 && code != 47 &&
             m_loop->GetWnd().GetBoxes()["ip"].size() < 21) {
    m_loop->GetWnd().GetBoxes()["ip"].push_back(static_cast<char>(code));
  }
  m_loop->GetWnd().SetObject("client", "box", 1, m_loop->GetWnd().GetBoxes()["ip"]);
}

void PortBoxCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetShow("settings", "port_status", false, 1);

  if (const size_t code = m_loop->GetWnd().GetEvent().getIf<Event::TextEntered>()->unicode;
    code == 8 && !m_loop->GetWnd().GetBoxes()["port"].empty()) {
    m_loop->GetWnd().GetBoxes()["port"].pop_back();
  } else if (code == 13) {
    PortCommand().Execute();
  } else if (code >= 48 && code <= 58 && m_loop->GetWnd().GetBoxes()["port"].size() < 5) {
    m_loop->GetWnd().GetBoxes()["port"].push_back(static_cast<char>(code));
  }
  m_loop->GetWnd().SetObject("settings", "port_box", 1, m_loop->GetWnd().GetBoxes()["port"]);
}

std::string IPClientCommand::m_ip_addr = R"(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))";
std::string IPClientCommand::m_ip_port =
    R"(([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))";
std::string IPClientCommand::m_ip_full =
    R"(^()" + m_ip_addr + R"(\.){3})" + m_ip_addr + R"(:)" + m_ip_port;
std::regex IPClientCommand::m_ip_regex(m_ip_full);
std::regex PortCommand::m_port_regex(IPClientCommand::m_ip_port);

void IPClientCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetShow("client", "status", false);

  auto [fst, snd] = ParseIp();
  if (fst.empty()) {
    m_loop->GetWnd().SetShow("client", "status", true, 1);
    return;
  }
  if (m_loop->GetNetwork().GetPort() == snd) {
    m_loop->GetWnd().SetShow("client", "status", true, 4);
    return;
  }
  std::cerr << fst;
  if (auto resolved = sf::IpAddress::resolve(fst)) {
    m_loop->GetNetwork().ClientConnect({resolved.value(), snd});
  } else {
    std::cerr << "Failed to resolve IP address: " << fst << std::endl;
  }
}

pair<string, size_t> IPClientCommand::ParseIp() {
  string text = m_loop->GetWnd().GetBoxes()["ip"];
  std::cerr << text;
  if (!std::regex_match(text, m_ip_regex)) {
    return {"", 0};
  }

  string ip_address;
  while (text[0] != ':') {
    ip_address.push_back(text[0]);
    text.erase(text.begin());
  }
  text.erase(text.begin());
  return {ip_address, std::stoi(text)};
}

void IPServerCommand::Execute(bool is_remote) {
  SetSceneCommand("server").Execute();
  m_loop->GetNetwork().ServerConnect();
}

void PortCommand::Execute(bool is_remote) {
  if (m_loop == nullptr || m_loop->GetNetwork().Connected()) {
    return;
  }
  m_loop->GetWnd().SetShow("settings", "port_status", false);

  const string text = m_loop->GetWnd().GetBoxes()["port"];
  if (!std::regex_match(text, m_port_regex)) {
    m_loop->GetWnd().SetShow("settings", "port_status", true, 1);
    return;
  }

  const size_t port_backup = m_loop->GetNetwork().GetPort();
  switch (m_loop->GetNetwork().UpdatePort(std::stoi(text))) {
    case Socket::Status::Done:
      m_loop->GetWnd().SetShow("settings", "port_status", true, 0);
      sf::sleep(sf::milliseconds(kMoveSleep));
      m_loop->GetWnd().SetShow("settings", "port_status", false, 0);
      break;
    default:
      m_loop->GetNetwork().UpdatePort(port_backup);
      m_loop->GetWnd().SetShow("settings", "port_status", true, 2);
  }
}

VolumeCommand::VolumeCommand(const CMDVolume type) : m_type(type) {
}

void VolumeCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetVolume(m_type);
}

SetSceneCommand::SetSceneCommand(const string& str) : Command(MyEventType::Closed), m_str(str) {
}

deque<string> SetSceneCommand::m_stack = {"menu"};

void DisconnectCommand::Execute(const bool is_remote) {
  m_loop->Clear();
  m_loop->GetNetwork().Disconnect(is_remote);
  if (m_loop->GetWnd().GetMusic("game").getStatus() == SoundSource::Status::Playing) {
    m_loop->GetWnd().GetMusic("game").stop();
    m_loop->GetWnd().GetMusic("main").play();
  }
}


void SetSceneCommand::Execute(bool is_remote) {
  const std::string str = m_str;

  m_loop->GetWnd().Post([loop = m_loop, str]() mutable {
    auto& wnd = loop->GetWnd();
    const auto& net = loop->GetNetwork();

    if (str == "ficha") {
      wnd.Ficha();
      return;
    }

    if (str == "adios" ||
        (str == "back" && !m_stack.empty() && m_stack.back() == "menu")) {
      DisconnectCommand().Execute(true);
      wnd.close();
      return;
    }

    if (str == "back") {
      if (!m_stack.empty()) {
        const auto& top = m_stack.back();
        const auto size = top.size();

        if (top == "disconnected") {
          m_stack.resize(3);
          DisconnectCommand().Execute(false);
        } else if (top == "won_0" || top == "won_1") {
          m_stack.resize(2);
          DisconnectCommand().Execute(true);
        } else if (top == "server" || top == "client" || top == "waiting" ||
                   (size > 0 && top.substr(0, size - 1) == "select_") ||
                   (size > 0 && top.substr(0, size - 1) == "play_")) {
          m_stack.resize(3);
          DisconnectCommand().Execute(true);
        }

        if (!m_stack.empty())
          m_stack.pop_back();
      }
    } else {
      m_stack.push_back(str);

      if (str == "settings") {
        if (net.Connected()) {
          wnd.SetShow(str, "port_save", false);
        } else {
          wnd.SetShow(str, "port_save", true);
        }
        wnd.GetBoxes()["port"] = bs::atos(net.GetPort());
        wnd.SetObject(str, "port_box", 1, wnd.GetBoxes()["port"]);
      }
    }

    if (!m_stack.empty())
      wnd.SetButtons(m_stack.back());
  });
}


CellCommand::CellCommand(Player* play, Cell* cell) : m_player(play), m_cell(cell) {
}

AddCellCommand::AddCellCommand(Player* play, Cell* cell) : CellCommand(play, cell) {
}

void AddCellCommand::Execute(bool is_remote) {
  m_loop->GetWnd().Post([this, is_remote]() {
    const std::string scene = "select_" + bs::atos(m_player->GetIndex());
    m_loop->GetWnd().SetShow(scene, "status", false);

    if (!IsValid()) {
      m_loop->GetWnd().SetShow(scene, "status", true, 1);
      return;
    }

    switch (m_cell->GetState()) {
      case CellState::Clear:
        m_player->m_ship_in_process.AddCell(m_cell);
        break;
      default:
        m_player->m_ship_in_process.EraseCell(m_cell);
        break;
    }

    m_loop->GetWnd().DrawObjects();
    if (!is_remote)
      Send();
  });
}

bool AddCellCommand::IsValid() const {
  return m_cell->GetState() == CellState::Clear || m_cell->GetState() == CellState::Chosen;
}

void AddCellCommand::Send() {
  const auto& c = m_cell->GetCoord();
  const size_t ind = static_cast<size_t>(c.x) * m_loop->kSize.y + c.y;
  m_loop->GetNetwork().Send("add_cell", bs::atos(ind));
}

AddShipCommand::AddShipCommand(Player* player) : m_player(player) {
}

void AddShipCommand::Execute(const bool is_remote) {
  m_loop->GetWnd().Post([this, is_remote]() {
    const std::string scene = "select_" + bs::atos(m_player->GetIndex());
    m_loop->GetWnd().SetShow(scene, "status", false);

    if (!IsValid()) {
      m_loop->GetWnd().SetShow(scene, "status", true, 2);
      return;
    }

    m_player->AddShip();

    if (!is_remote) {
      Network& net = m_loop->GetNetwork();
      net.Send("add_ship", "");
    }

    m_loop->GetWnd().SetShow(scene, "status", true, 0);
    sf::sleep(sf::milliseconds(kMoveSleep));
    m_loop->GetWnd().SetShow(scene, "status", false, 0);

    if (m_player->GetShipCount() == m_loop->kShips) {
      const auto play = m_loop->GetLocalPlayer();
      m_player->GetMField()->RemoveProhibited();
      m_loop->GetWnd().SetShow(
          "play_" + bs::atos(m_player->GetIndex()),
          "turn",
          true,
          (m_player->GetIndex() ^ 1)
          );

      if (m_player->GetRival()->GetShipCount() == m_loop->kShips) {
        m_loop->GetWnd().GetMusic("main").pause();
        m_loop->GetWnd().GetMusic("game").play();
        SetSceneCommand("play_" + bs::atos(play)).Execute();
      } else if (m_player->GetIndex() == m_loop->GetLocalPlayer()) {
        SetSceneCommand("waiting").Execute();
      }
    }

    m_loop->GetWnd().DrawObjects();
  });
}


bool AddShipCommand::IsValid() const {
  if (!m_player->GetShipInProcess()->IsClassic()) {
    return false;
  }
  return m_player->GetNumShips(m_player->GetShipInProcess()->GetSize()) <
         5 - m_player->GetShipInProcess()->GetSize();
}

void AddShipCommand::Send() {
  m_loop->GetNetwork().Send("add_ship");
}

ShootCommand::ShootCommand(Player* player, Cell* cell) : CellCommand(player, cell) {
}

void ShootCommand::Execute(const bool is_remote) {
  if (!is_remote) {
    const auto& c = m_cell->GetCoord();
    const size_t ind = static_cast<size_t>(c.x) * m_loop->kSize.y + c.y;
    m_loop->GetNetwork().Send("shoot", bs::atos(ind));
  }

  m_loop->GetWnd().Post([this, is_remote]() {
    if (!IsValid() ^ is_remote) {
      return;
    }

    const size_t play = m_loop->GetLocalPlayer();

    const ShotState shot_result = m_player->Shoot(m_cell);

    SetSceneCommand("play_" + bs::atos(play)).Execute(is_remote);

    if (m_player->GetRival()->GetShipCount() == 0) {
      SetSceneCommand("won_" + bs::atos(m_player->GetIndex() ^ play)).Execute(is_remote);
      m_loop->GetBlocked() = false;
      return;
    }

    if (shot_result == ShotState::Miss) {
      const size_t ind = (play == m_player->GetIndex()) ? 1 : 0;
      m_loop->GetBlocked() = !m_loop->GetBlocked();
      auto& wnd = m_loop->GetWnd();
      wnd.SetShow("play_" + bs::atos(play), "turn", false, ind ^ 1);
      wnd.SetShow("play_" + bs::atos(play), "turn", true, ind);
    }
  });
}


bool ShootCommand::IsValid() const {
  return m_loop->GetBlocked();
}

void ShootCommand::Send() {
  const auto& c = m_cell->GetCoord();
  const size_t ind = static_cast<size_t>(c.x) * m_loop->kSize.y + c.y;
  m_loop->GetNetwork().Send("shoot", bs::atos(ind));
}