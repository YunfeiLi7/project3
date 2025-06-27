#include "Enemy.h"
#include "Path.h"
#include "Cell.h"
#include "Tower.h"
#include <QPainter>
#include <QLineF>
#include <QtMath>
#include <QDebug>

Enemy::Enemy(Path* startPath, const QPixmap& image, int maxHp, float speed, int damage, float attackCooldown, int tileSize)
    : m_path(startPath),
      m_pathIdx(-1),
      m_nextCell(nullptr),
      m_speed(speed),
      m_blocked(false),
      m_atkTarget(nullptr),
      m_img(image),
      m_maxHp(maxHp),
      m_hp(maxHp),
      m_damage(damage),
      m_atkCooldown(attackCooldown),
      m_atkTimer(attackCooldown),
      m_tileSize(tileSize)
{
    //放缩图像
    if (!image.isNull()) {
        int desiredWidth = 32;
        int desiredHeight = 32;
        m_img = image.scaled(desiredWidth, desiredHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (m_img.isNull() && !image.isNull()) {
            m_img = image;
        }
    }
    advanceToNextCell();//设置第一个目标点

}

Enemy::~Enemy() {
}

void Enemy::advanceToNextCell() {
    if (!m_path) return;

    m_pathIdx++;//路径索引自增
    const auto& pathCells = m_path->getCells();

    if (m_pathIdx < pathCells.size()) {
        m_nextCell = pathCells[m_pathIdx];//更新目标格子
        //初始化第一个点
        if (m_pathIdx == 0) {
            m_pos = QPointF(
                m_nextCell->getCol() * m_tileSize + (m_tileSize - m_img.width()) / 2.0,
                m_nextCell->getRow() * m_tileSize + (m_tileSize - m_img.height()) / 2.0
            );
        }
    } else {
        //路径走完了置空指针
        m_nextCell = nullptr;

    }
}
//每帧更新状态
void Enemy::update(float deltaTime, const QVector<Tower*>& towersOnMap) {
    if (!isAlive()) return;

    if (hasReachedEnd()) {
        return;
    }

    if (!m_nextCell) {
        return;
    }
    //重置阻挡和攻击目标状态
    m_blocked = false;
    m_atkTarget = nullptr;
    int currentLogicRow = static_cast<int>((m_pos.y() + m_img.height() / 2.0) / m_tileSize);
    int currentLogicCol = static_cast<int>((m_pos.x() + m_img.width() / 2.0) / m_tileSize);
//检查前进道路是否被阻挡
    for (Tower* tower : towersOnMap) {
        //活着的近战塔才可以阻挡
        if (tower && !tower->isDestroyed() && tower->getType() == Tower::Type::MELEE) {
            if (tower->getRow() == m_nextCell->getRow() && tower->getCol() == m_nextCell->getCol()) {

                m_blocked = true;
                m_atkTarget = tower;
                break;
            } else if (tower->getRow() == currentLogicRow && tower->getCol() == currentLogicCol) {
                m_blocked = true;
                m_atkTarget = tower;
                break;
            }
        }
    }
//如果阻挡，执行攻击逻辑
    if (m_blocked && m_atkTarget) {
        m_atkTimer += deltaTime;
        if (m_atkTimer >= m_atkCooldown) {
            attackTargetTower(m_atkTarget);
            m_atkTimer = 0.0f;
        }
    } else {
        //如果没有阻挡那么朝着目标格子中心移动
        QPointF targetCenterPxPos(
            m_nextCell->getCol() * m_tileSize + (m_tileSize - m_img.width()) / 2.0,
            m_nextCell->getRow() * m_tileSize + (m_tileSize - m_img.height()) / 2.0
        );
        moveToTarget(targetCenterPxPos, deltaTime);
        //检测是否到达中心
        QPointF currentCenterPos = m_pos + QPointF(m_img.width() / 2.0, m_img.height() / 2.0);
        QPointF targetCellCenterForCompare = QPointF(m_nextCell->getCol() * m_tileSize + m_tileSize / 2.0,
                                                    m_nextCell->getRow() * m_tileSize + m_tileSize / 2.0);

        QLineF lineToCellCenter(currentCenterPos, targetCellCenterForCompare);
        //有些误差没关系
        if (lineToCellCenter.length() < qMax(2.0f, m_speed * deltaTime * 0.5f) ) {
            m_pos = targetCenterPxPos;
            advanceToNextCell();
        }
    }
}
//处理移动到目标的像素位置
void Enemy::moveToTarget(const QPointF& targetPxPos, float deltaTime) {
    QLineF lineToTarget(m_pos, targetPxPos);
    //避免抖动
    if (lineToTarget.length() < 1.0f) {
        m_pos = targetPxPos;
        return;
    }
    QPointF directionNormal = lineToTarget.unitVector().p2() - lineToTarget.unitVector().p1();
    float distanceToMoveThisFrame = m_speed * deltaTime;
    m_pos += directionNormal * distanceToMoveThisFrame;
}
void Enemy::attackTargetTower(Tower* tower) {
    if (tower && !tower->isDestroyed()) {

      tower->takeDamage(m_damage);
        //如果塔没了，重置阻挡状态，继续前进
        if (tower->isDestroyed()) {
            qDebug() << "Tower at (" << tower->getRow() << "," << tower->getCol() << ") was destroyed by an enemy.";
            m_blocked = false;
            m_atkTarget = nullptr;
        }
    }
}
//敌人受伤逻辑
void Enemy::takeDamage(int amount) {
    m_hp -= amount;
    if (m_hp < 0) {
        m_hp = 0;
    }

    if (!isAlive()) {
        qDebug() << "Enemy at (" << m_pos.x() << "," << m_pos.y() << ") DIED.";
    }
}
//检测是否到达终点
bool Enemy::hasReachedEnd() const {
    return !m_nextCell && m_path && (m_pathIdx >= m_path->getCells().size());
}
void Enemy::draw(QPainter* painter, int mapOffsetY, int tileSizeRef) const {
    if (!isAlive() || m_img.isNull()) return;
    //计算实际屏幕中的绘制坐标
    QPointF screenDrawPos(m_pos.x(), m_pos.y() + mapOffsetY);
    painter->drawPixmap(screenDrawPos.toPoint(), m_img);
    //绘制血条，绿色的，和最高血量按照百分比扣血
    QRectF healthBarBgRect(
        screenDrawPos.x() + (m_img.width() - HP_BAR_W) / 2.0,
        screenDrawPos.y() - HP_BAR_OFFSET_Y - HP_BAR_H,
        HP_BAR_W,
        HP_BAR_H
    );
    painter->setBrush(Qt::darkRed);
    painter->setPen(Qt::NoPen);
    painter->drawRect(healthBarBgRect);

    if (m_maxHp > 0) {
        float healthPercent = static_cast<float>(m_hp) / m_maxHp;
        QRectF healthBarFgRect(
            healthBarBgRect.left(),
            healthBarBgRect.top(),
            HP_BAR_W * healthPercent,
            HP_BAR_H
        );
        painter->setBrush(Qt::green);
        painter->drawRect(healthBarFgRect);
    }
    //让血条更清晰一些
    painter->setPen(QPen(Qt::black, 0.5));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(healthBarBgRect);
}
//获得敌人的碰撞箱
QRectF Enemy::getBoundingBox(int mapOffsetY, int tileSizeRef) const {
    return QRectF(m_pos.x(), m_pos.y() + mapOffsetY, m_img.width(), m_img.height());
}
//获得敌人中心点坐标
QPointF Enemy::getCenterPos() const {

    return m_pos + QPointF(m_img.width() / 2.0, m_img.height() / 2.0);
}