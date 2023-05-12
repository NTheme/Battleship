#include "../lib/command.hpp"

#include "../lib/button.hpp"
#include "../lib/cell.hpp"
#include "../lib/game.hpp"
#include "../lib/player.hpp"
#include "../lib/window.hpp"

GameLoop* Command::m_loop = nullptr;

Command::Command() : m_type(Event::Count) {}

Command::Command(const Event::EventType& type) : m_type(type) {}

const Event::EventType& Command::GetType() { return m_type; }

void IPBoxCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetShow("client", "status", false);

  size_t code = m_loop->GetWnd().GetEvent().text.unicode;

  if (code == 8 && !m_loop->GetWnd().GetBoxes()["ip"].empty()) {
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

  size_t code = m_loop->GetWnd().GetEvent().text.unicode;

  if (code == 8 && !m_loop->GetWnd().GetBoxes()["port"].empty()) {
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
std::regex IPClientCommand::m_ip_regex(IPClientCommand::m_ip_full);
std::regex PortCommand::m_port_regex(IPClientCommand::m_ip_port);

void IPClientCommand::Execute(bool is_remote) {
  m_loop->GetWnd().SetShow("client", "status", false);

  auto ip_got = ParseIp();
  if (ip_got.first.empty()) {
    m_loop->GetWnd().SetShow("client", "status", true, 1);
    return;
  }
  if (m_loop->GetNetwork().GetPort() == ip_got.second) {
    m_loop->GetWnd().SetShow("client", "status", true, 4);
    return;
  }
  m_loop->GetNetwork().ClientConnect(ip_got);
}

pair<string, size_t> IPClientCommand::ParseIp() {
  string text = m_loop->GetWnd().GetBoxes()["ip"];
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
  if (m_loop->GetNetwork().Connected()) {
    return;
  }
  m_loop->GetWnd().SetShow("settings", "port_status", false);

  string text = m_loop->GetWnd().GetBoxes()["port"];
  if (!std::regex_match(text, m_port_regex)) {
    m_loop->GetWnd().SetShow("settings", "port_status", true, 1);
    return;
  }

  size_t port_backup = m_loop->GetNetwork().GetPort();
  switch (m_loop->GetNetwork().UpdatePort(std::stoi(text))) {
    case Socket::Done:
      m_loop->GetWnd().SetShow("settings", "port_status", true, 0);
      sf::sleep(sf::milliseconds(kMoveSleep));
      m_loop->GetWnd().SetShow("settings", "port_status", false, 0);
      break;
    default:
      m_loop->GetNetwork().UpdatePort(port_backup);
      m_loop->GetWnd().SetShow("settings", "port_status", true, 2);
  }
}

VolumeCommand::VolumeCommand(CMDVolume type) : m_type(type) {}

void VolumeCommand::Execute(bool is_remote) { m_loop->GetWnd().SetVolume(m_type); }

SetSceneCommand::SetSceneCommand(const string& m_str) : Command(Event::Closed), m_str(m_str) {}

deque<string> SetSceneCommand::m_stack = {"menu"};

void DisconnectCommand::Execute(bool is_remote) {
  m_loop->Clear();
  m_loop->GetNetwork().Disconnect(is_remote);
  if (m_loop->GetWnd().GetMusic("game").getStatus() == SoundSource::Playing) {
    m_loop->GetWnd().GetMusic("game").stop();
    m_loop->GetWnd().GetMusic("main").play();
  }
}

void SetSceneCommand::Execute(bool is_remote) {
  if (m_str == "ficha") {
    m_loop->GetWnd().Ficha();
    return;
  }
  if (m_str == "adios" || (m_str == "back" && m_stack.back() == "menu")) {
    DisconnectCommand().Execute(true);
    m_loop->GetWnd().close();
    return;
  }

  if (m_str == "back") {
    auto size = m_stack.back().size();
    if (m_stack.back() == "disconnected") {
      m_stack.resize(3);
      DisconnectCommand().Execute(false);
    } else if ((m_stack.back() == "won_0" || m_stack.back() == "won_1")) {
      m_stack.resize(2);
      DisconnectCommand().Execute(true);
    } else if (m_stack.back() == "server" || m_stack.back() == "client" ||
               m_stack.back() == "waiting" || m_stack.back().substr(0, size - 1) == "select_" ||
               m_stack.back().substr(0, size - 1) == "play_") {
      m_stack.resize(3);
      DisconnectCommand().Execute(true);
    }
    m_stack.pop_back();
  } else {
    m_stack.push_back(m_str);
    if (m_str == "settings") {
      if (m_loop->GetNetwork().Connected()) {
        m_loop->GetWnd().SetShow(m_str, "port_save", false);
      } else {
        m_loop->GetWnd().SetShow(m_str, "port_save", true);
      }
      m_loop->GetWnd().GetBoxes()["port"] = bs::atos(m_loop->GetNetwork().GetPort());
      m_loop->GetWnd().SetObject(m_str, "port_box", 1, m_loop->GetWnd().GetBoxes()["port"]);
    }
  }
  m_loop->GetWnd().SetButtons(m_stack.back());
}

CellCommand::CellCommand(Player* play, Cell* cell) : m_player(play), m_cell(cell) {}

AddCellCommand::AddCellCommand(Player* play, Cell* cell) : CellCommand(play, cell) {}

void AddCellCommand::Execute(bool is_remote) {
  string scene = "select_" + bs::atos(m_player->GetIndex());
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
  }
  m_loop->GetWnd().DrawObjects();
  if (!is_remote) {
    Send();
  }
}

bool AddCellCommand::IsValid() const {
  return m_cell->GetState() == CellState::Clear || m_cell->GetState() == CellState::Chosen;
}

void AddCellCommand::Send() {
  size_t ind = m_cell->GetCoord().x * m_loop->kSize.y + m_cell->GetCoord().y;
  m_loop->GetNetwork().Send("add_cell", bs::atos(ind));
}

AddShipCommand::AddShipCommand(Player* play) : m_player(play) {}

void AddShipCommand::Execute(bool is_remote) {
  string scene = "select_" + bs::atos(m_player->GetIndex());
  m_loop->GetWnd().SetShow(scene, "status", false);
  if (!IsValid()) {
    m_loop->GetWnd().SetShow(scene, "status", true, 2);
    return;
  }

  m_player->AddShip();
  if (!is_remote) {
    Send();
  }

  m_loop->GetWnd().SetShow(scene, "status", true, 0);
  sf::sleep(sf::milliseconds(kMoveSleep));
  m_loop->GetWnd().SetShow(scene, "status", false, 0);

  if (m_player->GetShipCount() == m_loop->kShips) {
    auto play = m_loop->GetLocalPlayer();
    m_player->GetMField()->RemoveProhibited();
    m_loop->GetWnd().SetShow("play_" + bs::atos(m_player->GetIndex()), "turn", true,
                             m_player->GetIndex() ^ 1);

    if (m_player->GetRival()->GetShipCount() == m_loop->kShips) {
      m_loop->GetWnd().GetMusic("main").pause();
      m_loop->GetWnd().GetMusic("game").play();
      SetSceneCommand("play_" + bs::atos(play)).Execute();
    } else if (m_player->GetIndex() == m_loop->GetLocalPlayer()) {
      SetSceneCommand("waiting").Execute();
    }
    m_loop->GetWnd().DrawObjects();
  }
}

bool AddShipCommand::IsValid() const {
  if (!m_player->GetShipInProcess()->IsClassic()) {
    return false;
  }
  return m_player->GetNumShips(m_player->GetShipInProcess()->GetSize()) <
         5 - m_player->GetShipInProcess()->GetSize();
}

void AddShipCommand::Send() { m_loop->GetNetwork().Send("add_ship"); }

ShootCommand::ShootCommand(Player* play, Cell* cell) : CellCommand(play, cell) {}

void ShootCommand::Execute(bool is_remote) {
  if (!IsValid() ^ is_remote) {
    return;
  }
  if (!is_remote) {
    Send();
  }

  size_t play = m_loop->GetLocalPlayer();
  ShotState shot_result = m_player->Shoot(m_cell);
  SetSceneCommand("play_" + bs::atos(play)).Execute();
  if (m_player->GetRival()->GetShipCount() == 0) {
    SetSceneCommand("won_" + bs::atos(m_player->GetIndex() ^ play)).Execute();
    m_loop->GetBlocked() = false;
  }
  if (shot_result == ShotState::Miss) {
    size_t ind = play == m_player->GetIndex() ? 1 : 0;
    m_loop->GetBlocked() = !m_loop->GetBlocked();
    m_loop->GetWnd().SetShow("play_" + bs::atos(play), "turn", false, ind ^ 1);
    m_loop->GetWnd().SetShow("play_" + bs::atos(play), "turn", true, ind);
  }
}

bool ShootCommand::IsValid() const { return m_loop->GetBlocked(); }

void ShootCommand::Send() {
  size_t ind = m_cell->GetCoord().x * m_loop->kSize.y + m_cell->GetCoord().y;
  m_loop->GetNetwork().Send("shoot", bs::atos(ind));
}
