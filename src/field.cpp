#include "../lib/field.hpp"

#include "../lib/cell.hpp"
#include "../lib/ship.hpp"

Field::Field(const Vector2u& size) : m_size(size), m_cells(deque<deque<Cell>>(m_size.x)) {
  for (size_t i = 0; i < m_size.x; ++i) {
    for (size_t j = 0; j < m_size.y; ++j) {
      m_cells[i].push_back(Cell(Vector2u(i, j)));
    }
  }
}

void MyField::Clear() {
  for (int i = 0; i < m_size.x; ++i) {
    for (int j = 0; j < m_size.y; ++j) {
      m_cells[i][j].SetState(CellState::Clear);
      m_cells[i][j].SetShip(nullptr);
    }
  }
}

void Field::LinkField(Field* other) {
  for (size_t i = 0; i < m_size.x; ++i) {
    for (size_t j = 0; j < m_size.y; ++j) {
      m_cells[i][j].SetTwins(&other->m_cells[i][j]);
    }
  }
}

Cell* Field::GetCell(const Vector2u& coord) { return &m_cells[coord.x][Vector2u(coord).y]; }

void Field::SurroundExcept(Cell* cell, CellState around, CellState except) {
  size_t current_w = cell->GetCoord().x;
  size_t current_l = cell->GetCoord().y;
  if (current_w != 0) {
    m_cells[current_w - 1][current_l].SetStateExcept(around, except);
    if (current_l != 0) {
      m_cells[current_w - 1][current_l - 1].SetStateExcept(around, except);
    }
    if (current_l != m_size.y - 1) {
      m_cells[current_w - 1][current_l + 1].SetStateExcept(around, except);
    }
  }
  if (current_w != m_size.x - 1) {
    m_cells[current_w + 1][current_l].SetStateExcept(around, except);
    if (current_l != 0) {
      m_cells[current_w + 1][current_l - 1].SetStateExcept(around, except);
    }
    if (current_l != m_size.y - 1) {
      m_cells[current_w + 1][current_l + 1].SetStateExcept(around, except);
    }
  }
  if (current_l != 0) {
    m_cells[current_w][current_l - 1].SetStateExcept(around, except);
  }
  if (current_l != m_size.y - 1) {
    m_cells[current_w][current_l + 1].SetStateExcept(around, except);
  }
}

MyField::MyField(const Vector2u& size) : Field(size) {
  for (size_t i = 0; i < size.x; ++i) {
    for (size_t j = 0; j < size.y; ++j) {
      m_cells[i][j].SetState(CellState::Clear);
    }
  }
}

void MyField::SetShip(Ship* ship) {
  for (Cell* cell : ship->GetCells()) {
    cell->SetState(CellState::Alive);
    cell->SetShip(ship);
    SurroundExcept(cell, CellState::Prohibited, CellState::Alive);
  }
}

void MyField::RemoveProhibited() {
  for (size_t i = 0; i < m_size.x; ++i) {
    for (size_t j = 0; j < m_size.y; ++j) {
      if (m_cells[i][j].GetState() == CellState::Prohibited) {
        m_cells[i][j].SetState(CellState::Clear);
      }
    }
  }
}

RivalField::RivalField(const Vector2u& size) : Field(size) {}

ShotState RivalField::UpdateShot(Cell* cell) {
  ShotState shot_result;
  Cell* twin = cell->GetTwin();
  if (twin->GetState() == CellState::Alive) {
    cell->SetState(CellState::Harmed);
    twin->SetState(CellState::Harmed);
    twin->GetShip()->DecrementHealth();
    if (!twin->GetShip()->IsAlive()) {
      shot_result = ShotState::Kill;
      for (Cell* killed_cell : twin->GetShip()->GetCells()) {
        killed_cell->SetState(CellState::Killed);
        killed_cell->GetTwin()->SetState(CellState::Killed);
        SurroundExcept(killed_cell, CellState::Clear, CellState::Killed);
      }
    } else {
      shot_result = ShotState::Harm;
    }
  } else if (twin->GetState() == CellState::Clear) {
    cell->SetState(CellState::Missed);
    twin->SetState(CellState::Missed);
    shot_result = ShotState::Miss;
  } else {
    shot_result = ShotState::Miss;
  }
  return shot_result;
}

void RivalField::Clear() {
  for (int i = 0; i < m_size.x; ++i) {
    for (int j = 0; j < m_size.y; ++j) {
      m_cells[i][j].SetState(CellState::Unknown);
      m_cells[i][j].SetShip(nullptr);
    }
  }
}
