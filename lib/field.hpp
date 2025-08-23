#pragma once

#include "cell.hpp"
#include "common.hpp"

class Field
{
public:
 explicit Field(const Vector2u& size);
 virtual ~Field() = default;
 virtual void Clear() = 0;
 void LinkField(Field* other);

 Cell* GetCell(const Vector2u& coord);
 void SurroundExcept(Cell* cell, CellState around, CellState except);

protected:
 Vector2f m_size;
 deque<deque<Cell>> m_cells;
};

class MyField final : public Field
{
public:
 explicit MyField(const Vector2u& size);
 void Clear() override;
 void SetShip(Ship* ship);
 void RemoveProhibited();
};

class RivalField final : public Field
{
public:
 explicit RivalField(const Vector2u& size);
 void Clear() override;
 ShotState UpdateShot(Cell* cell);
};
