//
// Created by 21048 on 25-6-26.
//

#ifndef TOWER_H
#define TOWER_H

#include <QPixmap>
#include <QString>


class Tower {
public:
    enum class Type {
        NONE,
        MELEE,
        RANGED
    };

//绘制上层塔的卡片
    struct CardInfo {
        Type type = Type::NONE;
        QString name;
        QPixmap cardImg;
        QPixmap towerImg;
        int cost = 0;
    };

    // 构造函数，需要位置信息
    Tower(int row, int col);
    virtual ~Tower() = default;

    // 获取位置和图像
    int getRow() const { return m_row; }
    int getCol() const { return m_col; }
    const QPixmap& getImage() const { return m_image; }

protected:
    int m_row;      // 塔所在的行
    int m_col;      // 塔所在的列
    QPixmap m_image; // 塔的图像
};

// 近战塔
class MeleeTower : public Tower {
public:
    MeleeTower(int row, int col);
};

// 远程塔
class RangedTower : public Tower {
public:
    RangedTower(int row, int col);
};

#endif // TOWER_H