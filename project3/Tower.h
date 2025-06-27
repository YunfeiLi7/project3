//
// Created by 21048 on 25-6-26.
//

#ifndef TOWER_H
#define TOWER_H

#include <QString>
#include <QPixmap>
#include <QPoint>
#include <QDebug>
#include <QVector>
class Bullet;
class Enemy;

class Tower {
public:
    enum class Type {
        NONE,
        MELEE,
        RANGED
    };


    struct CardInfo {
        Type type = Type::NONE;
        QString name;
        QPixmap cardImg;
        QPixmap towerImg;
        int cost = 0;
    };//绘制上方塔的卡片样式


    Tower(Type type, int r, int c, int startHealth = 100);
    virtual ~Tower() = default;//虚函数的形式给出析构函数，方便后期析构


    Type getType() const { return m_type; }
    int getRow() const { return m_row; }
    int getCol() const { return m_col; }
    const QPixmap& getImage() const { return m_image; }
    int getHealth() const { return m_health; }
    void takeDamage(int damage) { m_health -= damage; if (m_health < 0) m_health = 0; } // 示例：受伤//近战塔才会受伤
    bool isDestroyed() const { return m_health <= 0; }

protected:
    Type m_type;
    int m_row;
    int m_col;
    QPixmap m_image;
    int m_health;
};



#endif // TOWER_H
