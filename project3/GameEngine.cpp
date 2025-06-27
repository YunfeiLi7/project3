//
// Created by 21048 on 25-6-26.
//
#include "GameEngine.h"
#include <QDebug>

GameEngine::GameEngine(QObject *parent)
    : QObject(parent), m_gameOver(false), m_isPaused(false)
{
    // 连接计时器的 timeout 信号到 gameLoop 槽函数，用来不停更新游戏
    connect(&m_gameTimer, &QTimer::timeout, this, &GameEngine::gameLoop);
    // 设置游戏循环的间隔
    m_gameTimer.setInterval(1000 / 60);
}

GameEngine::~GameEngine()
{
    qDeleteAll(m_towers);
    m_towers.clear();
}

bool GameEngine::startGame()
{
    m_gameOver = false;
    m_isPaused = false;
    m_gameTimer.start();
    emit gameUpdated(); // 发出更新信号绘制初始地图
    return true;
}

void GameEngine::pauseGame()
{//后期可以设计暂停按钮之类的
    if (!m_gameOver && !m_isPaused) {
        m_isPaused = true;
        m_gameTimer.stop();
        qDebug() << "GE: Game paused.";
        emit gameUpdated(); // 发出信号更新
    }
}

void GameEngine::resumeGame()
{//继续游戏
    if (!m_gameOver && m_isPaused) {
        m_isPaused = false;
        m_gameTimer.start();

    }
}

void GameEngine::endGame()
{
    if (!m_gameOver) {
        m_gameOver = true;
        m_gameTimer.stop();
    }
}

void GameEngine::gameLoop()
{// 发出信号，告诉重绘
    emit gameUpdated();
}

Tower::CardInfo GameEngine::getTowerStaticData(Tower::Type type) const {
    Tower::CardInfo info;
    info.type = type;
    //对于近战塔和远程塔的性质描述
    switch (type) {
        case Tower::Type::MELEE:
            info.cost = 50;
        break;
        case Tower::Type::RANGED:
            info.cost = 100;
        break;
        default:
            info.cost = 99999;
        break;
    }
    return info;
}
// 判断地图某个位置是否可以放置塔的逻辑
bool GameEngine::canPlaceTower(Tower::Type type, int row, int col) const {
    const Cell* cell = m_map.getCell(row, col);
    if (!cell) { return false; }

    for (const Tower* existingTower : m_towers) {
        if (existingTower->getRow() == row && existingTower->getCol() == col) {
            return false;
        }
    }

    if (type == Tower::Type::MELEE) {
        return cell->isPath();
    } else if (type == Tower::Type::RANGED) {
        return cell->isBuildable(); // 远程塔只能建在草地上
    }
    return false;
}

// 部署塔的逻辑
bool GameEngine::deployTower(int row, int col, Tower::Type type)
{
    if (!canPlaceTower(type, row, col)) {
        return false;
    }

    Tower::CardInfo towerData = getTowerStaticData(type);
    if (m_playerMoney < towerData.cost) {
        return false;
    }

    Tower* newTower = nullptr;
    if (type == Tower::Type::MELEE) {
        newTower = new MeleeTower(row, col);
    } else if (type == Tower::Type::RANGED) {
        newTower = new RangedTower(row, col);
    }

    if (newTower) {
        m_towers.append(newTower);
        m_playerMoney -= towerData.cost;
        emit gameUpdated(); // 发出信号，通知UI刷新
        return true;
    }
    return false;
}