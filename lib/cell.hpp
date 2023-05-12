#pragma once

#include "common.hpp"

class Cell {
 public:
  Cell(const Vector2u& coord);
  ~Cell() = default;

  const Vector2u& GetCoord() const;
  CellState GetState() const;
  Ship* GetShip() const;
  Cell* GetTwin() const;
  sf::RectangleShape* GetShape() const;
  void SetState(CellState state);
  void SetStateExcept(CellState state, CellState except);
  void SetShip(Ship* ship);
  void SetTwins(Cell* other);
  void SetShape(sf::RectangleShape* shape);

 private:
  Vector2u m_coord;
  CellState m_state;
  Ship* m_ship;
  Cell* m_cell_t;
  sf::RectangleShape* m_shape;

  void UpdateColor();
};

bool CellComparator(const Cell* cell_1, const Cell* cell_2);
