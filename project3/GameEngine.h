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

    signals:

        void gameUpdated();

    protected slots:
        void gameLoop();

private:
    Map m_map;         //地图
    QTimer m_gameTimer; // 游戏循环计时器
    bool m_gameOver;    // 游戏是否结束
    bool m_isPaused;    // 游戏是否暂停
};

#endif // GAMEENGINE_H
