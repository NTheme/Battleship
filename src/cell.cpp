#include "../lib/cell.hpp"

Cell::Cell(const Vector2u& coord)
    : m_coord(coord), m_ship(nullptr), m_cell_t(nullptr), m_shape(nullptr) {
  SetState(CellState::Unknown);
}

const Vector2u& Cell::GetCoord() const { return m_coord; }

CellState Cell::GetState() const { return m_state; }

Ship* Cell::GetShip() const { return m_ship; }

Cell* Cell::GetTwin() const { return m_cell_t; }

sf::RectangleShape* Cell::GetShape() const { return m_shape; }

void Cell::SetState(CellState state) {
  m_state = state;
  if (m_shape != nullptr) {
    UpdateColor();
  }
}

void Cell::SetStateExcept(CellState state, CellState except) {
  if (GetState() != except) {
    SetState(state);
  }
};

void Cell::SetShip(Ship* ship) { m_ship = ship; }

void Cell::SetTwins(Cell* other) {
  m_cell_t = other;
  other->m_cell_t = this;
}

void Cell::SetShape(sf::RectangleShape* shape) { m_shape = shape; }

void Cell::UpdateColor() {
  switch (m_state) {
    case CellState::Alive:
      m_shape->setFillColor(Color(0, 255, 255));
      break;
    case CellState::Chosen:
      m_shape->setFillColor(Color(0, 255 - 120, 0));
      break;
    case CellState::Clear:
      m_shape->setFillColor(Color(255, 120, 255));
      break;
    case CellState::Prohibited:
      m_shape->setFillColor(Color(255, 0, 0));
      break;
    case CellState::Harmed:
      m_shape->setFillColor(Color(100, 100, 0));
      break;
    case CellState::Missed:
      m_shape->setFillColor(Color(0, 0, 255));
      break;
    case CellState::Killed:
      m_shape->setFillColor(Color(0, 0, 0));
      break;
    case CellState::Unknown:
      m_shape->setFillColor(Color(255, 255, 255));
      break;
  }
}

bool CellComparator(const Cell* cell_1, const Cell* cell_2) {
  return cell_1->GetCoord().x < cell_2->GetCoord().x ||
         (cell_1->GetCoord().x == cell_2->GetCoord().x &&
          cell_1->GetCoord().y < cell_2->GetCoord().y);
}
