#pragma once

#include "common.hpp"

class Cell {
public:
  explicit Cell(const Vector2u& coord);
  ~Cell() = default;

  [[nodiscard]] const Vector2u& GetCoord() const;
  [[nodiscard]] CellState GetState() const;
  [[nodiscard]] Ship* GetShip() const;
  [[nodiscard]] Cell* GetTwin() const;
  [[nodiscard]] RectangleShape* GetShape() const;
  void SetState(CellState state);
  void SetStateExcept(CellState state, CellState except);
  void SetShip(Ship* ship);
  void SetTwins(Cell* other);
  void SetShape(RectangleShape* shape);

private:
  Vector2u m_coord;
  CellState m_state;
  Ship* m_ship;
  Cell* m_cell_t;
  RectangleShape* m_shape;

  void UpdateColor() const;
};

bool CellComparator(const Cell* cell_1, const Cell* cell_2);