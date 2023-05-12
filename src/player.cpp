#include "../lib/player.hpp"

#include "../lib/cell.hpp"

Player::Player(size_t index, const Vector2u& size)
    : m_index(index),
      m_ship_count(0),
      m_field_m(MyField(size)),
      m_field_r(RivalField(size)),
      m_ships(deque<deque<Ship>>(5)),
      m_rival(nullptr) {}

void Player::Clear() {
  m_field_m.Clear();
  m_field_r.Clear();
  for (int i = 0; i < 5; ++i) {
    m_ships[i].clear();
  }
  m_ship_in_process.Clear();
  m_ship_count = 0;
}

size_t Player::GetIndex() const { return m_index; }

size_t Player::GetShipCount() const { return m_ship_count; }

MyField* Player::GetMField() { return &m_field_m; }

RivalField* Player::GetRField() { return &m_field_r; }

void Player::DecrementShipCount() { --m_ship_count; }

void Player::AddShip() {
  m_ships[m_ship_in_process.GetSize()].push_back(m_ship_in_process);
  ++m_ship_count;
  m_field_m.SetShip(&*m_ships[m_ship_in_process.GetSize()].rbegin());
  m_ship_in_process.Clear();
}

ShotState Player::Shoot(Cell* cell) {
  ShotState shot_result = m_field_r.UpdateShot(cell);
  if (shot_result == ShotState::Kill) {
    m_rival->DecrementShipCount();
  }
  return shot_result;
}

const Ship* Player::GetShipInProcess() const { return &m_ship_in_process; }

size_t Player::GetNumShips(size_t size) const { return m_ships[size].size(); }

void Player::LinkWithRival(Player* rival) {
  m_rival = rival;
  rival->m_rival = this;
  m_field_m.LinkField(&rival->m_field_r);
  m_field_r.LinkField(&rival->m_field_m);
}

Player* Player::GetRival() { return m_rival; }
