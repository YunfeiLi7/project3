//
// Created by 21048 on 25-6-25.
//
#include "Path.h"
#include <QDebug>
//path这个类主要辅佐map，map里面可以有多条路径，每条路径通过addcell的方式进行构建
Path::Path() {
}
void Path::addCell(Cell* cell) {
    if (cell) {
        m_pathCells.append(cell);
        if (m_pathCells.size() > 1) {
            m_pathCells[m_pathCells.size() - 2]->setIsEndPoint(false);//把非尾部设置为false
        }
        cell->setIsEndPoint(true);//每次加入设置一个false
        if (m_pathCells.size() == 1) {
            cell->setIsStartPoint(true);//加了一个就设置开头
        }
    }
}
Cell* Path::getStartCell() const {
    if (m_pathCells.isEmpty()) {
        return nullptr;
    }
    return m_pathCells.first();
}
Cell* Path::getEndCell() const {
    if (m_pathCells.isEmpty()) {
        return nullptr;
    }
    return m_pathCells.last();
}
Cell* Path::getNextCell(Cell* currentCell) const {
    if (!currentCell || m_pathCells.isEmpty()) {
        return nullptr;
    }
    for (int i = 0; i < m_pathCells.size(); ++i) {
        if (m_pathCells[i] == currentCell) {
            if (i < m_pathCells.size() - 1) {
                return m_pathCells[i + 1];
            } else {
                return nullptr;
            }
        }
    }
    return nullptr;
}
