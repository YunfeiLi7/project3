//
// Created by 21048 on 25-6-26.
//

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>
#include "map.h"
#include "Tower.h"


class GameEngine : public QObject
{
    Q_OBJECT

public:
    explicit GameEngine(QObject *parent = nullptr);
    ~GameEngine();

    // 对于游戏流程控制
    bool startGame();
    void pauseGame();
    void resumeGame();
    void endGame();

    // 地图访问
    const Map& getMap() const { return m_map; }
//关于塔的放置逻辑
    int getPlayerMoney() const { return m_playerMoney; }
    const QVector<Tower*>& getTowers() const { return m_towers; }
    bool canPlaceTower(Tower::Type type, int row, int col) const;
    bool deployTower(int row, int col, Tower::Type type);
    Tower::CardInfo getTowerStaticData(Tower::Type type) const;

    signals:

        void gameUpdated();

    protected slots:
        void gameLoop();

private:
    Map m_map;         //地图
    QTimer m_gameTimer; // 游戏循环计时器
    bool m_gameOver;    // 游戏是否结束
    bool m_isPaused;    // 游戏是否暂停
    int m_playerMoney;          // 玩家金钱
    QVector<Tower*> m_towers;   // 存储已放置的塔
};

#endif // GAMEENGINE_H
