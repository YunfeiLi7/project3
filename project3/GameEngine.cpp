#include "GameEngine.h"
#include <QRandomGenerator>
#include <QDebug>
GameEngine::GameEngine(QObject *parent)
    : QObject(parent),
m_map(),
m_gameTimer(this),
m_isGameActive(false),
m_isPaused(false),
m_playerMoney(250),
m_playerBaseHealth(5),
m_enemiesReachedEndCount(0),
m_currentWaveNumber(0),
m_enemiesToSpawnThisWave(0),
m_enemySpawnTimer(this),
m_timeBetweenWaves(15.0f),
m_waveCooldownTimer(0.0f)
{//关联主计时器
    connect(&m_gameTimer, &QTimer::timeout, this, &GameEngine::gameLoop);
    m_gameTimer.setInterval(33);
    connect(&m_enemySpawnTimer, &QTimer::timeout, this, &GameEngine::spawnEnemyInCurrentWave);

}

GameEngine::~GameEngine() {
    m_gameTimer.stop();
    m_enemySpawnTimer.stop();
    qDeleteAll(m_towers);
    m_towers.clear();
    qDeleteAll(m_enemies);
    m_enemies.clear();
    qDeleteAll(m_bullets);
    m_bullets.clear();
}


bool GameEngine::startGame()
{
    if (m_map.getRows() <= 0 || m_map.getCols() <= 0) {  return false; }
    //游戏开始，开始初始化一些变量
    m_isGameActive = true;
    m_isPaused = false;
    m_playerMoney = 250;
    m_playerBaseHealth = 5;
    m_enemiesReachedEndCount = 0;


    m_currentWaveNumber = 0;
    m_waveCooldownTimer = 3.0f;
    m_enemiesToSpawnThisWave = 0;
    if(m_enemySpawnTimer.isActive()) m_enemySpawnTimer.stop();


    m_gameTimer.start();
    emit gameUpdated();
    return true;
}
//暂停游戏
void GameEngine::pauseGame()
{
    if (m_isGameActive && !m_isPaused) {
        m_isPaused = true;
        m_gameTimer.stop();
        if (m_enemySpawnTimer.isActive()) {
            m_enemySpawnTimer.stop();
        }
        emit gameUpdated();
    }
}
//继续游戏
void GameEngine::resumeGame()
{

    if (m_isGameActive && m_isPaused) {
        m_isPaused = false;
        m_gameTimer.start();
        if (m_enemiesToSpawnThisWave > 0) {
            m_enemySpawnTimer.start();
        }
        emit gameUpdated();
    }
}
//游戏结束
void GameEngine::endGame()
{
    m_gameTimer.stop();
    m_enemySpawnTimer.stop();
    m_isGameActive = false;
    m_isPaused = false;
    qDeleteAll(m_towers);
    m_towers.clear();
    qDeleteAll(m_enemies);
    m_enemies.clear();
    qDeleteAll(m_bullets);
    m_bullets.clear();

    emit gameUpdated();
}
//游戏主循环逻辑
void GameEngine::gameLoop()
{

    if (!m_isGameActive || m_isPaused) {
        return;
    }

    float deltaTime = m_gameTimer.interval() / 1000.0f;

    updateEnemies(deltaTime);
    updateBullets(deltaTime);
    //更新每一个塔的状态
    for (Tower* tower : m_towers) {
        if (tower && !tower->isDestroyed()) {
            tower->update(deltaTime);
            Bullet* newBullet = tower->attack(m_enemies);
            if (newBullet) {
                m_bullets.append(newBullet);
            }
        }
    }

    cleanupEntities();
    updateGameStatus(deltaTime);

    emit gameUpdated();
}
//用于gamewindow绘制上面的卡片类型
Tower::CardInfo GameEngine::getTowerStaticData(Tower::Type type) const {
    Tower::CardInfo info;
    info.type = type;
    switch (type) {
        case Tower::Type::MELEE:
            info.name = "近战守卫";
            info.cost = 50;

            break;
        case Tower::Type::RANGED:
            info.name = "远程射手";
            info.cost = 100;
            break;
        case Tower::Type::NONE:
        default:
            info.name = "未知";
            info.cost = 99999;
            qWarning("GE: getTowerStaticData for NONE or unknown type: %d", static_cast<int>(type));
            break;
    }
    return info;
}
int GameEngine::getStaticTowerRange(Tower::Type type) const {
    switch (type) {
        case Tower::Type::MELEE:
            return 0;
        case Tower::Type::RANGED:
            return 3;
        default:
            return 0;
    }
}

//看地图的哪一个位置可以放置什么类型的塔
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
        if (!cell->isPath() && cell->getType() == Cell::Grass) { // 假设远程塔只能建在草地上
            int dr[] = {-1, 1, 0, 0}; int dc[] = {0, 0, -1, 1};
            for (int i = 0; i < 4; ++i) {
                const Cell* adjacentCell = m_map.getCell(row + dr[i], col + dc[i]);
                if (adjacentCell && adjacentCell->isPath()) {
                    return true;
                }
            }
        }
        return false;
    }

    return false;
}
//放置成功或者失败
bool GameEngine::deployTower(int row, int col, Tower::Type type)
{
    if (!m_isGameActive) {  return false; }

    if (!canPlaceTower(type, row, col)) {
        return false;
    }

    Tower::CardInfo towerData = getTowerStaticData(type); // 获取成本等信息

    if (m_playerMoney < towerData.cost) {
        return false;
    }

    Tower* newTower = nullptr;
    if (type == Tower::Type::MELEE) {
        newTower = new MeleeTower(row, col);
    } else if (type == Tower::Type::RANGED) {
        newTower = new RangedTower(row, col);
    } else {
        return false;
    }

    if (newTower) {
        m_towers.append(newTower);
        m_playerMoney -= towerData.cost;
        emit gameUpdated();
        return true;
    }

    return false;
}


//处理下一波敌人
void GameEngine::startNextWave(){
    m_currentWaveNumber++;
    m_enemiesToSpawnThisWave = 5 + m_currentWaveNumber * 3;
    int spawnIntervalMs = 1600 - m_currentWaveNumber * 80;
    if (spawnIntervalMs < 400) {
        spawnIntervalMs = 400;
    }

    m_enemySpawnTimer.setInterval(spawnIntervalMs);
    if (m_enemiesToSpawnThisWave > 0) {
        m_enemySpawnTimer.start();
    }
    m_waveCooldownTimer = 0;
}

void GameEngine::spawnEnemyInCurrentWave() {
    if (!m_isGameActive || m_enemiesToSpawnThisWave <= 0) {
        m_enemySpawnTimer.stop();
        return;
    }

    const auto& allPaths = m_map.getPaths();
    if (allPaths.isEmpty()) {
        m_enemySpawnTimer.stop();
        return;
    }

    Path* chosenPath = nullptr;
    //敌人在三条路径随意生成
    if (!allPaths.isEmpty()) {
        int randomPathIndex = QRandomGenerator::global()->bounded(allPaths.size());
        chosenPath = const_cast<Path*>(&allPaths[randomPathIndex]); // 获取随机选中的路径指针

    }
    if (!chosenPath || chosenPath->getCells().isEmpty()) {


        m_enemiesToSpawnThisWave--;
        if (m_enemiesToSpawnThisWave <= 0) {
            m_enemySpawnTimer.stop();
            m_waveCooldownTimer = m_timeBetweenWaves; // 进入波次间冷却

        }
        return;
    }
    QPixmap enemyImg;
    enemyImg.load(":/images/zombie.png");
    //敌人血量逐渐增加
    int enemyHP = 80 + m_currentWaveNumber * 15 + m_currentWaveNumber * m_currentWaveNumber * 2;
    Enemy* newEnemy = new Enemy(chosenPath, enemyImg, enemyHP, 25.0f, 10, 1.5f, 40); // 示例参数
    m_enemies.append(newEnemy);

    m_enemiesToSpawnThisWave--;
    //后期加强敌人，生成速度加快，数量变多
    if (m_enemiesToSpawnThisWave <= 0) {
        m_enemySpawnTimer.stop();
        float nextCooldown = 15.0f - m_currentWaveNumber * 0.5f;
        if (nextCooldown < 5.0f) {
            nextCooldown = 5.0f;
        }
        m_waveCooldownTimer = nextCooldown;
    }
}

void GameEngine::updateEnemies(float deltaTime) {

    for (Enemy* enemy : m_enemies) {

        if (enemy) {
            enemy->update(deltaTime, m_towers);
        }
    }
}
//更新子弹
void GameEngine::updateBullets(float deltaTime) {
    for (Bullet* bullet : m_bullets) {
        if (bullet) {
            bullet->update(deltaTime);
            if (bullet->hasHitTarget()) {
                //如果达到了，执行伤害逻辑
                Enemy* target = bullet->getTarget();
                if (target && target->isAlive()) {
                    target->takeDamage(bullet->getDamage());
                }
            }
        }
    }
}
//如果有敌人到底部，那么生命扣除，生命到0触发结束逻辑
void GameEngine::handleEnemyReachedEnd(Enemy* enemy) {
    m_playerBaseHealth--;
    m_enemiesReachedEndCount++;

    if (m_playerBaseHealth <= 0 || m_enemiesReachedEndCount >= MAX_ENEMIES_TO_END) {

        triggerGameOver(false);
    }
}

//清除敌人，防止访问空指针导致报错
void GameEngine::cleanupEntities() {

    QVector<Enemy*> enemiesToDelete;
    for (Enemy* enemy : m_enemies) {
        if (!enemy->isAlive() || enemy->hasReachedEnd()) {
            enemiesToDelete.append(enemy);
        }
    }

    for (Enemy* deadEnemy : enemiesToDelete) {
        for (Tower* tower : m_towers) {
            if (auto rangedTower = dynamic_cast<RangedTower*>(tower)) {
                if (rangedTower->getCurrentTarget() == deadEnemy) {
                    rangedTower->clearTarget();
                }
            }
        }
        for (Bullet* bullet : m_bullets) {
            if (bullet->getTarget() == deadEnemy) {
                bullet->targetKilled();
            }
        }
        if (deadEnemy->hasReachedEnd()) {
            handleEnemyReachedEnd(deadEnemy);
        } else {
            m_playerMoney += 150;
        }
        m_enemies.removeAll(deadEnemy);
        delete deadEnemy;
    }

    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (m_bullets[i]->hasHitTarget()) {
            delete m_bullets[i];
            m_bullets.removeAt(i);
        }
    }

    for (int i = m_towers.size() - 1; i >= 0; --i) {
        if (m_towers[i]->isDestroyed()) {
            delete m_towers[i];
            m_towers.removeAt(i);
        }
    }
}
//更新游戏状态
void GameEngine::updateGameStatus(float deltaTime) {
    if (!m_isGameActive) return;
    if (m_enemiesToSpawnThisWave <= 0 && m_enemies.isEmpty()) {
        if (m_currentWaveNumber >= TOTAL_WAVES) {

            triggerGameOver(true);
            return;
        }
        if (m_waveCooldownTimer > 0) {
            m_waveCooldownTimer -= deltaTime;
            if (m_waveCooldownTimer <= 0) {
                startNextWave();
            }
        } else if (m_currentWaveNumber == 0){
             startNextWave();
        }
    }


    if (m_playerBaseHealth <= 0) {

        triggerGameOver(false);

    }
}
//塔觉醒的逻辑
bool GameEngine::awakenTower(int row, int col) {

    if (m_playerMoney < AWAKENING_BEAN_COST) {
        return false;
    }

    Tower* targetTower = nullptr;
    for (Tower* tower : m_towers) {
        if (tower->getRow() == row && tower->getCol() == col) {
            targetTower = tower;
            break;
        }
    }

    if (!targetTower) {
        return false;
    }
    RangedTower* rangedTarget = dynamic_cast<RangedTower*>(targetTower);
    if (!rangedTarget) {
        return false;
    }
    if (rangedTarget->isAwakened()) {
        return false;
    }
    m_playerMoney -= AWAKENING_BEAN_COST;
    rangedTarget->awaken();
    emit gameUpdated();
    return true;
}

void GameEngine::triggerGameOver(bool playerWon) {
    if (!m_isGameActive) {
        return;
    }
    m_gameTimer.stop();
    m_enemySpawnTimer.stop();


    m_isGameActive = false;
    m_isPaused = false;

//传到gameengine，再根据具体是赢是输gameengine来判断输出什么
    emit gameOverSignal(playerWon);
}