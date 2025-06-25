//
// Created by 21048 on 25-6-25.
//

#ifndef PATH_H
#define PATH_H
#include <QVector>
#include <QPoint>
#include "Cell.h"
class Path {
public:
    Path();
    void addCell(Cell* cell);//路径加入一小块一小块的指针
    const QVector<Cell*>& getCells() const { return m_pathCells; }//一条路上对应的所有点
    Cell* getStartCell() const;//找起始点
    Cell* getEndCell() const;//找末端点
    Cell* getNextCell(Cell* currentCell) const;//便于寻路

private:
    QVector<Cell*> m_pathCells;//容器用来装cell
};

#endif //PATH_H
