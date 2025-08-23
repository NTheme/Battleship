#pragma once

#include "common.hpp"
#include "network.hpp"
#include "player.hpp"
#include "window.hpp"

class GameLoop {
public:
  explicit GameLoop(const Vector2u& size, size_t ships = 10);

  ~GameLoop();

  void Go();
  void Clear();

  GameWindow& GetWnd();
  Network& GetNetwork();

  void LaunchNetwork();
  void Terminate();

  bool& GetBlocked();
  [[nodiscard]] size_t GetLocalPlayer() const;
  void SetLocalPlayer(size_t local_player);

  const size_t kShips;
  const Vector2u kSize;

private:
  std::array<Player, 2> m_players;
  std::deque<Command*> m_turns;
  GameWindow m_window;
  Network m_network;

  std::thread m_network_thr;
  std::atomic<bool> m_netRunning{false};

  size_t m_local_player = 0;
  bool m_blocked = false;

  void ProcessNetwork();
};