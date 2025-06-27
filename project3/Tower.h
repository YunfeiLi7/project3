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
    //塔的类型，进展和远程
    enum class Type {
        NONE,
        MELEE,
        RANGED
    };

  //用于绘制卡牌
    struct CardInfo {
        Type type = Type::NONE;
        QString name;
        QPixmap cardImg;
        QPixmap towerImg;
        int cost = 0;
    };


    Tower(Type type, int r, int c, int startHealth = 100);
    virtual ~Tower() = default;


    Type getType() const { return m_type; }//便于其他类获取类型
    int getRow() const { return m_row; }//获取行数
    int getCol() const { return m_col; }//获取列数
    const QPixmap& getImage() const { return m_image; }
    int getHealth() const { return m_health; }//塔的血条
    void takeDamage(int damage) { m_health -= damage; if (m_health < 0) m_health = 0; } // 受伤
    bool isDestroyed() const { return m_health <= 0; }//看是不是要塔消失

    virtual void update(float deltaTime);//及时更新
    virtual Bullet* attack(const QVector<Enemy*>& enemies) = 0;//管理子弹类
    virtual int getAttackRange() const = 0;//获取攻击范围，便于绘制拖拽的范围以及后期寻找敌人

protected:
    Type m_type;
    int m_row;
    int m_col;
    QPixmap m_image;
    int m_health;
};


class MeleeTower : public Tower {
public:

    MeleeTower(int r, int c);
    Bullet* attack(const QVector<Enemy*>& enemies) override;//暂时不攻击
    int getAttackRange() const override;
};


class RangedTower : public Tower {
public:

    RangedTower(int r, int c);
    void update(float deltaTime) override;//上传
    Bullet* attack(const QVector<Enemy*>& enemies) override;//攻击函数，先获取范围内的敌人
    int getAttackRange() const override;
    Enemy* getCurrentTarget() const { return m_currentTarget; }//锚定一个敌人
    void clearTarget() { m_currentTarget = nullptr; }//目标死了清除
    bool isAwakened() const { return m_isAwakened; }//觉醒
    void awaken();//觉醒函数
private:

    int m_damage;//伤害
    float m_attackCooldown;//攻击冷却
    float m_attackTimer;
    QPixmap m_bulletPixmap;//图片

    Enemy* m_currentTarget;//此时锁定敌人
    bool m_isAwakened;//是否觉醒
    QPixmap m_awakenedBulletPixmap;//觉醒图标
};

#endif // TOWER_H