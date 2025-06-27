#ifndef ENEMY_H
#define ENEMY_H
#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QDebug>
#include "Path.h"
#include "Cell.h"

#include <QPainter>
class Tower;
class Enemy {
public:

    Enemy(Path* startPath, const QPixmap& image, int maxHp, float speed, int damage, float attackCooldown, int tileSize);
    ~Enemy();
    void update(float deltaTime, const QVector<Tower*>& towers);//更新函数
    void draw(QPainter* painter, int mapOffsetY, int tileSize) const;//敌人的绘制逻辑
    bool isAlive() const { return m_hp > 0; }//判断函数，判断敌人是否或者
    bool hasReachedEnd() const;//判断是否到末尾，如果到了那么生命值扣除一格
    QPointF getPosition() const { return m_pos; }//算位置
    QPointF getCenterPos() const;//寻找中心点
    void takeDamage(int amount);//受伤的逻辑
    QRectF getBoundingBox(int mapOffsetY, int tileSize) const;//算碰撞箱，尤其是周围的一圈逻辑


private:
    Path* m_path;//存储路径，方便生成
    int m_pathIdx;//每条路的生成个数
    Cell* m_nextCell;//下一个cell走哪个，存储一下
    QPointF m_pos;//当前位置
    float m_speed;//速度
    bool m_blocked;//是否堵住，就是是否停止
    Tower* m_atkTarget;//对于近战塔展开攻击

    QPixmap m_img;//僵尸图片
    int m_maxHp;//最大血量
    int m_hp;//当前血量
    int m_damage;//伤害
    float m_atkCooldown;//攻击冷却
    float m_atkTimer;
    int m_tileSize;//大小
    void moveToTarget(const QPointF& targetCenterPxPos, float deltaTime);//移动的函数
    void advanceToNextCell();//移动的下一个目标格子
    void attackTargetTower(Tower* tower);//执行对塔的近战攻击


    const int HP_BAR_H = 5;
    const int HP_BAR_W = 30;
    const int HP_BAR_OFFSET_Y = 5;
};

#endif // ENEMY.H