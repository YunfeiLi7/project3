//
// Created by 21048 on 25-6-25.
//

#ifndef MAP_H
#define MAP_H
#include <QVector>
#include <QString>
#include "Cell.h"
#include "Path.h"

class Map {
public:
    Map();

    ~Map();
    void clearMap();//删除地图
    int getRows() const { return m_rows; }//得到地图行
    int getCols() const { return m_cols; }//得到地图列
    Cell* getCell(int r, int c) const;//获取地图上某一个格子的指针
    const QVector<Path>& getPaths() const { return m_paths; }//获得路径



private:
    int m_rows;
    int m_cols;
    QVector<QVector<Cell*>> m_grid;//存一下地图中每个cell的指针
    QVector<Path> m_paths;//存一下道路
    void initializeDefaultMapData();
};
#endif //MAP_H
