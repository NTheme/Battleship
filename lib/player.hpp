#pragma once

#include "common.hpp"
#include "field.hpp"
#include "ship.hpp"

class Player {
  friend class AddCellCommand;

 public:
  Player(size_t index, const Vector2u& size);
  ~Player() = default;

  void Clear();
  size_t GetIndex() const;
  size_t GetShipCount() const;
  MyField* GetMField();
  RivalField* GetRField();
  void DecrementShipCount();
  void AddShip();
  ShotState Shoot(Cell* cell);
  const Ship* GetShipInProcess() const;
  size_t GetNumShips(size_t size) const;
  void LinkWithRival(Player* rival);
  Player* GetRival();

 private:
  size_t m_index;
  size_t m_ship_count;
  MyField m_field_m;
  RivalField m_field_r;
  deque<deque<Ship>> m_ships;
  Ship m_ship_in_process;
  Player* m_rival;
};
