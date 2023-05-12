#include "../lib/game.hpp"

#include "../lib/command.hpp"

GameLoop::GameLoop(const Vector2u& size, size_t ships)
    : kShips(ships),
      kSize(size),
      m_players(array<Player, 2>{Player(0, size), Player(1, size)}),
      m_window(m_players, kSize),
      m_network(this),
      m_network_thr(&GameLoop::ProcessNetwork, this) {
  Command::m_loop = this;
  m_players[0].LinkWithRival(&m_players[1]);
}

void GameLoop::ProcessNetwork() {
  while (m_window.isOpen() && m_network.Connected()) {
    m_network.GetCommand()->Execute(true);
  }
}

void GameLoop::Go() {
  while (m_window.isOpen()) {
    m_window.GetCommand()->Execute();
  }
  m_network_thr.terminate();
}

void GameLoop::Clear() {
  for (int pl = 0; pl < 2; ++pl) {
    m_players[pl].Clear();
  }
}

GameWindow& GameLoop::GetWnd() { return m_window; }

Network& GameLoop::GetNetwork() { return m_network; }

void GameLoop::LaunchNetwork() { m_network_thr.launch(); }

void GameLoop::Terminate() { m_network_thr.terminate(); }

bool& GameLoop::GetBlocked() { return m_blocked; }

size_t GameLoop::GetLocalPlayer() const { return m_local_player; }

void GameLoop::SetLocalPlayer(size_t local_player) { m_local_player = local_player; }
