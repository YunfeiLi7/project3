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
