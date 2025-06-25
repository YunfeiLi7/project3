//
// Created by 21048 on 25-6-25.
//
#include "Cell.h"

Cell::Cell(int row, int col, TileType type)
    : m_row(row),
    m_col(col),
    m_type(type),
    m_isStartPoint(false),
    m_isEndPoint(false)
{
    m_isPath = (m_type == PathStone);
    m_isBuildable = (m_type == Grass);
    //直接在构造函数中就判断这个格子属于什么类型的，方便后面建造
}
void Cell::setType(TileType type) {
    m_type = type;
    m_isPath = (m_type == PathStone);
    m_isBuildable = (m_type == Grass);
}





