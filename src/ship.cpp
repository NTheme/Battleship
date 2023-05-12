#include "../lib/ship.hpp"

#include "../lib/cell.hpp"

Ship::Ship(const std::deque<Cell*>& chosen_cells)
    : m_health(chosen_cells.size()), m_cells(chosen_cells) {
  std::sort(m_cells.begin(), m_cells.end(), CellComparator);
}

void Ship::Clear() {
  m_health = 0;
  m_cells.clear();
}

void Ship::DecrementHealth() {
  if (m_health > 0) {
    --m_health;
  }
}

bool Ship::IsAlive() const { return m_health > 0; }

const deque<Cell*>& Ship::GetCells() const { return m_cells; }

size_t Ship::GetSize() const { return m_cells.size(); }

void Ship::AddCell(Cell* cell) {
  ++m_health;
  m_cells.push_back(cell);
  cell->SetState(CellState::Chosen);
  size_t new_cell_index = GetSize() - 1;
  while (new_cell_index > 0 &&
         !CellComparator(m_cells[new_cell_index - 1], m_cells[new_cell_index])) {
    std::swap(m_cells[new_cell_index], m_cells[new_cell_index - 1]);
    --new_cell_index;
  }
}

void Ship::EraseCell(Cell* cell) {
  auto cell_location = find(m_cells.begin(), m_cells.end(), cell);
  if (cell_location != m_cells.end()) {
    if (cell->GetState() != CellState::Killed && cell->GetState() != CellState::Harmed) {
      --m_health;
    }
    m_cells.erase(find(m_cells.begin(), m_cells.end(), cell));
    cell->SetState(CellState::Clear);
  }
}

bool Ship::IsClassic() const {
  if (GetSize() == 1) {
    return true;
  }
  if (GetSize() > 4 || GetSize() == 0) {
    return false;
  }
  bool vertical = false;
  if (m_cells[0]->GetCoord().y == m_cells[1]->GetCoord().y) {
    vertical = true;
  }
  for (size_t i = 0; i < GetSize() - 1; ++i) {
    if ((vertical && m_cells[i + 1]->GetCoord().y == m_cells[i + 1]->GetCoord().y &&
         m_cells[i]->GetCoord().x + 1 == m_cells[i + 1]->GetCoord().x) ||
        (!vertical && m_cells[i]->GetCoord().x == m_cells[i + 1]->GetCoord().x &&
         m_cells[i]->GetCoord().y + 1 == m_cells[i + 1]->GetCoord().y)) {
      continue;
    }
    return false;
  }
  return true;
}
