//
// Created by 21048 on 25-6-25.
//

#ifndef CELL_H
#define CELL_H
#include <QPoint>

class Unit;

class Cell {
public:
    enum TileType {
        Empty = 0,//
        Grass = 1,//远程塔可以建立的位置
        PathStone = 2,//路径，僵尸走的还有近战塔可以放置的
        Obstacle = 3//什么都不能放置的
    };

    Cell(int row, int col, TileType type = Empty);

    int getRow() const { return m_row; }
    int getCol() const { return m_col; }

    QPoint getCoords() const { return QPoint(m_col, m_row); }//取点
    TileType getType() const { return m_type; }//取类型，方便之后写放置的逻辑
    void setType(TileType type); //看后面需不需要改变路径的类型
    bool isPath() const{return m_isPath;};//看一下是不是路
    bool isBuildable() const {return m_isBuildable;};//可不可以搭建
    bool isStartPoint() const { return m_isStartPoint; }//起始路径，方便后期写僵尸出现的逻辑
    void setIsStartPoint(bool isStart) { m_isStartPoint = isStart; }//起始路径
    bool isEndPoint() const { return m_isEndPoint; }
    void setIsEndPoint(bool isEnd) { m_isEndPoint = isEnd; }

private:
    int m_row;
    int m_col;
    TileType m_type;
    bool m_isPath;
    bool m_isBuildable;
    bool m_isStartPoint;
    bool m_isEndPoint;
};
#endif //CELL_H
