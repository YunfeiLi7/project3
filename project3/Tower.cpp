//
// Created by 21048 on 25-6-26.
//
#include "Tower.h"
#include "Bullet.h"
#include "Enemy.h"
Tower::Tower(Type type, int r, int c, int startHealth)
    : m_type(type),
      m_row(r),
      m_col(c),
      m_health(startHealth)
{

}
void Tower::update(float deltaTime) {

}


MeleeTower::MeleeTower(int r, int c)//暂时近战塔只用于卡位，不负责攻击
    : Tower(Tower::Type::MELEE, r, c, 150)
{

    m_image.load(":/images/light.png");

}

Bullet* MeleeTower::attack(const QVector<Enemy*>& /*enemies*/) {
    return nullptr;
}

int MeleeTower::getAttackRange() const {
    return 0;
}

RangedTower::RangedTower(int r, int c)
: Tower(Tower::Type::RANGED, r, c, 100),m_damage(15),
  m_attackCooldown(1.0f),
  m_attackTimer(0.0f),
m_currentTarget(nullptr),
m_isAwakened(false)
{

    m_image.load(":/images/bottle.png");//加载塔
    QPixmap originalBulletPixmap;
originalBulletPixmap.load(":/images/bullet.png");//加载子弹
    m_bulletPixmap = originalBulletPixmap.scaled(10, 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void RangedTower::update(float deltaTime) {//更新的逻辑
    if (m_attackTimer > 0) {
        m_attackTimer -= deltaTime;
    }
}
void RangedTower::awaken() {
    if (m_isAwakened) {
        //不会重复觉醒
        return;
    }

    m_isAwakened = true;
//觉醒

    m_damage *= 2;//伤害变高
    m_attackCooldown *= 0.6f;//攻速加快


    m_image.load(":/images/bottle_awakened.png");
    QPixmap originalAwakenedBullet;

originalAwakenedBullet.load(":/images/bullet_awakened.png");//觉醒子弹图片加载
        m_awakenedBulletPixmap = originalAwakenedBullet.scaled(12, 12, Qt::KeepAspectRatio, Qt::SmoothTransformation);

}
//攻击敌人逻辑
Bullet* RangedTower::attack(const QVector<Enemy*>& enemies) {
    if (m_attackTimer > 0) {
        return nullptr;
    }

    Enemy* target = nullptr;
    //获取图像中心，并且根据这个还有塔的范围去寻找敌人
    QPointF towerCenterPos((getCol() + 0.5) * 40, (getRow() + 0.5) * 40);
    int rangePixels = getAttackRange() * 40;
    if (m_currentTarget) {
        if (!m_currentTarget->isAlive() || QLineF(towerCenterPos, m_currentTarget->getCenterPos()).length() > rangePixels) {
            m_currentTarget = nullptr;
        }
    }
    if (!m_currentTarget) {
        double closestDist = 99999.0;//为了获取最近的敌人
        for (Enemy* enemy : enemies) {
            if (enemy && enemy->isAlive()) {
                double dist = QLineF(towerCenterPos, enemy->getCenterPos()).length();
                if (dist <= rangePixels && dist < closestDist) {
                    closestDist = dist;
                    m_currentTarget = enemy;
                }
            }
        }
    }
    if (m_currentTarget) {
        m_attackTimer = m_attackCooldown;
        const QPixmap& bulletToShow = m_isAwakened ? m_awakenedBulletPixmap : m_bulletPixmap;
        return new Bullet(towerCenterPos, m_currentTarget, m_damage, bulletToShow);
    }

    return nullptr;

}

int RangedTower::getAttackRange() const {
     return m_isAwakened ? 5 : 3;//觉醒扩大攻击范围
}