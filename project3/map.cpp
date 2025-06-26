//
// Created by 21048 on 25-6-25.
//
#include "Map.h"
#include <QDebug>

Map::Map()
    : m_rows(0), m_cols(0)
{
    initializeDefaultMapData();//构建地图
}

Map::~Map() {
    clearMap();
}

void Map::clearMap() {
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            if (m_grid[i][j]) {
                delete m_grid[i][j];
                m_grid[i][j] = nullptr;
            }
        }
        m_grid[i].clear();
    }
    m_grid.clear();
    m_rows = 0;
    m_cols = 0;
    m_paths.clear();
}

Cell* Map::getCell(int r, int c) const {
    if (r >= 0 && r < m_rows && c >= 0 && c < m_cols) {
        return m_grid[r][c];
    }
    return nullptr;
}
void Map::initializeDefaultMapData() {
    //首先实现逻辑地图，在后面的类中实现绘图逻辑
    clearMap();
    m_rows = 15;
    m_cols = 20;
    int defaultMapGrid[15][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1,1,1,1,1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,2,2,2,2,2},
{1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,2,0,1,0,0},
{1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 1, 1, 1, 1,2,1,1,1,1},
{1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2,2,0,1,1,1},
{2, 2, 2, 2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,1,1,1,1,1},
{1, 1, 1, 1, 1, 1, 1, 1, 1,1, 0, 1, 1, 1, 1,1,1,1,1,1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,2,2,2,2,2},
{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,0,1,1,1,1},
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1,1,1,1,1},
{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,1,1,1,1,1},
{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1,1,1,1,1,1},
{2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 2, 2, 2, 2,2,2,2,2,2},
{1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 1, 1, 1,0,1,1,1,1},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,1,1,1,1,1},
    };
    //完成逻辑地图
    m_grid.resize(m_rows);
    for (int r = 0; r < m_rows; ++r) {
        m_grid[r].resize(m_cols);
        for (int c = 0; c < m_cols; ++c) {
            m_grid[r][c] = new Cell(r, c, static_cast<Cell::TileType>(defaultMapGrid[r][c]));
        }
    }

 Path path;
    //一共三条道路，每条道路里面采用addcell和getcell两种方式进行对于路径的构建
path.addCell(getCell(12,19));
    path.addCell(getCell(12,18));
    path.addCell(getCell(12,17));
    path.addCell(getCell(12,16));
    path.addCell(getCell(12,15));
    path.addCell(getCell(12,14));
    path.addCell(getCell(12,13));
    path.addCell(getCell(12,12));
    path.addCell(getCell(12,11));
    path.addCell(getCell(13,11));
    path.addCell(getCell(13,10));
    path.addCell(getCell(13,9));
    path.addCell(getCell(13,8));
    path.addCell(getCell(13,7));
    path.addCell(getCell(13,6));
    path.addCell(getCell(12,6));
    path.addCell(getCell(12,5));
    path.addCell(getCell(12,4));
    path.addCell(getCell(12,3));
    path.addCell(getCell(12,2));
    path.addCell(getCell(12,1));
    path.addCell(getCell(12,0));
    path.getStartCell()->setIsStartPoint(true);
    path.getEndCell()->setIsEndPoint(true);
   m_paths.append(path);

    Path path2;
    path2.addCell(getCell(7,19));
    path2.addCell(getCell(7,18));
    path2.addCell(getCell(7,17));
    path2.addCell(getCell(7,16));
    path2.addCell(getCell(7,15));
    path2.addCell(getCell(7,14));
    path2.addCell(getCell(8,14));
    path2.addCell(getCell(8,13));
    path2.addCell(getCell(8,12));
    path2.addCell(getCell(8,11));
    path2.addCell(getCell(8,10));
    path2.addCell(getCell(8,9));
    path2.addCell(getCell(8,8));
    path2.addCell(getCell(8,7));
    path2.addCell(getCell(8,6));
    path2.addCell(getCell(8,5));
    path2.addCell(getCell(8,4));
    path2.addCell(getCell(8,3));
    path2.addCell(getCell(8,2));
    path2.addCell(getCell(8,1));
    path2.addCell(getCell(8,0));
    path2.getStartCell()->setIsStartPoint(true);
    path2.getEndCell()->setIsEndPoint(true);
    m_paths.append(path2);
    Path path3;
    path3.addCell(getCell(1,19));
    path3.addCell(getCell(1,18));
    path3.addCell(getCell(1,17));
    path3.addCell(getCell(1,16));
    path3.addCell(getCell(1,15));
    path3.addCell(getCell(2,15));
    path3.addCell(getCell(3,15));
    path3.addCell(getCell(4,15));
    path3.addCell(getCell(4,14));
    path3.addCell(getCell(4,13));
    path3.addCell(getCell(4,12));
    path3.addCell(getCell(4,11));
    path3.addCell(getCell(4,10));
    path3.addCell(getCell(3,10));
    path3.addCell(getCell(3,9));
    path3.addCell(getCell(3,8));
    path3.addCell(getCell(3,7));
    path3.addCell(getCell(4,7));
    path3.addCell(getCell(4,6));
    path3.addCell(getCell(4,5));
    path3.addCell(getCell(4,4));
    path3.addCell(getCell(5,4));
    path3.addCell(getCell(5,3));
    path3.addCell(getCell(5,2));
    path3.addCell(getCell(5,1));
    path3.addCell(getCell(5,0));
    path3.getStartCell()->setIsStartPoint(true);
    path3.getEndCell()->setIsEndPoint(true);
    m_paths.append(path3);
}

