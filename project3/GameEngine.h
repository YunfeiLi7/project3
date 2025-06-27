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
#include "Enemy.h"
#include "Bullet.h"
class GameEngine : public QObject
{
    Q_OBJECT

public:
    explicit GameEngine(QObject *parent = nullptr);

    ~GameEngine();

    // 游戏流程控制
    bool startGame();
    void pauseGame();
    void resumeGame();
    void endGame();

    //  数据获取，私有成员
    const Map& getMap() const { return m_map; }
    const QVector<Tower*>& getTowers() const { return m_towers; }
    const QVector<Enemy*>& getEnemies() const { return m_enemies; }
    const QVector<Bullet*>& getBullets() const { return m_bullets; }
    int getPlayerMoney() const { return m_playerMoney; }
    int getPlayerHealth() const { return m_playerBaseHealth; }
    int getCurrentWaveNumber() const { return m_currentWaveNumber; }
    int getTotalWaves() const { return TOTAL_WAVES; }
    int getStaticTowerRange(Tower::Type type) const;
    int getAwakeningBeanCost() const { return AWAKENING_BEAN_COST; }
    bool isPaused() const { return m_isPaused; }
    bool isActive() const { return m_isGameActive; }

    //玩家操作
    bool deployTower(int row, int col, Tower::Type type);//放置塔楼
    bool canPlaceTower(Tower::Type type, int row, int col) const;//是否能放置塔
    bool awakenTower(int row, int col);//觉醒


signals:
    void gameUpdated();
    void gameOverSignal(bool playerWon);

protected slots:

    void gameLoop();

private:
    const int TOTAL_WAVES = 20;           // 总波数
    const int AWAKENING_BEAN_COST = 250;  // 觉醒豆的成本
    Map m_map;                          // 游戏地图
    QTimer m_gameTimer;                 // 游戏主循环计时器
    bool m_isGameActive;                // 游戏是否正在进行
    bool m_isPaused;                    // 游戏是否已暂停
    int m_playerMoney;                  // 玩家金钱
    int m_playerBaseHealth;             // 总的生命值
    int m_enemiesReachedEndCount;       // 到达终点的敌人数量
    const int MAX_ENEMIES_TO_END = 10;  // 允许到达终点的最大敌人数量
    QVector<Tower*> m_towers;           // 存储所有塔
    QVector<Bullet*> m_bullets;         // 存储所有子弹
    QVector<Enemy*> m_enemies;          // 存储所有敌人
    int m_currentWaveNumber;            // 当前波数
    int m_enemiesToSpawnThisWave;       // 当前波还需生成的敌人数量
    QTimer m_enemySpawnTimer;           // 控制敌人生成间隔的计时器
    float m_timeBetweenWaves;           // 每波之间的基础冷却时间
    float m_waveCooldownTimer;          // 当前波次间冷却的剩余时间

    Tower::CardInfo getTowerStaticData(Tower::Type type) const;//用于绘制
    void startNextWave();//生成下一波敌人
    void spawnEnemyInCurrentWave();//当前这波敌人
    void updateEnemies(float deltaTime);//更新敌人
    void updateBullets(float deltaTime);//更新子弹
    void updateGameStatus(float deltaTime);//游戏状态更新
    void handleEnemyReachedEnd(Enemy* enemy);//处理敌人到头逻辑
    void cleanupEntities();//清除
    void triggerGameOver(bool playerWon);//游戏中止处理
};

#endif // GAMEENGINE_H