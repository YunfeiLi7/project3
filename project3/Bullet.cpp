//
// Created by 21048 on 25-6-26.
//
#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QDebug>
Bullet::Bullet(QPointF startPos, Enemy* target, int damage, const QPixmap& image, float speed)
    : m_pos(startPos),
      m_target(target),
      m_pixmap(image),
      m_damage(damage),
      m_speed(speed),
      m_hasHit(false)
{//调整子弹位置
    m_pos -= QPointF(m_pixmap.width() / 2.0, m_pixmap.height() / 2.0);
}
//更新子弹状态
void Bullet::update(float deltaTime) {
    if (m_hasHit || !m_target || !m_target->isAlive()) {
        m_hasHit = true;
        return;
    }
//获取敌方中心点坐标
    QPointF targetPos = m_target->getCenterPos();
    //计算子弹当前中心点坐标
    QPointF bulletCenter = m_pos + QPointF(m_pixmap.width() / 2.0, m_pixmap.height() / 2.0);
    QLineF lineToTarget(bulletCenter, targetPos);
    //计算每一帧移动距离
    float distanceToMove = m_speed * deltaTime;
    //击中逻辑
    if (lineToTarget.length() <= distanceToMove) {
        m_hasHit = true;
        return;
    }
    //方向向量
    QPointF direction = lineToTarget.unitVector().p2() - lineToTarget.unitVector().p1();
    m_pos += direction * distanceToMove;
}
void Bullet::draw(QPainter* painter, int mapOffsetY) const {
    //只有没有击中才能绘制子弹
    if (!m_hasHit) {
        QPointF screenDrawPos(m_pos.x(), m_pos.y() + mapOffsetY);
        painter->drawPixmap(screenDrawPos.toPoint(), m_pixmap);
    }
}