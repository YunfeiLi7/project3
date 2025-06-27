//
// Created by 21048 on 25-6-26.
//

#ifndef BULLET_H
#define BULLET_H
#include <QPointF>
#include <QPixmap>
#include <QPainter>
class Enemy;

class Bullet {
public:
    Bullet(QPointF startPos, Enemy* target, int damage, const QPixmap& image, float speed = 150.0f);
    void update(float deltaTime);//更新子弹状态
    void draw(QPainter* painter, int mapOffsetY) const;//绘制子弹
    bool hasHitTarget() const { return m_hasHit; }//判断是否碰到
    Enemy* getTarget() const { return m_target; }//每个子弹知道所对应的敌人
    int getDamage() const { return m_damage; }//获取子弹伤害
    void targetKilled() {
        m_target = nullptr;
        m_hasHit = true;
    }
private:
    QPointF m_pos;//子弹位置
    Enemy* m_target;//子弹目标
    QPixmap m_pixmap;//子弹图片
    int     m_damage;//子弹伤害
    float   m_speed;//子弹速度
    bool    m_hasHit;//子弹是否打击到目标
};

#endif // BULLET_H