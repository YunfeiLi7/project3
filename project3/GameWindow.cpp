#include "GameWindow.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include "GameEngine.h"
#include <QApplication>
#include <QTimer>
GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
      m_isDragging(false),
      m_dragType(Tower::Type::NONE),
      m_dragRange(0),
      m_paletteHeight(0),
m_pauseButton(nullptr),
m_isGameOver(false)
{

    grassPixmap.load(":/images/grass.png");
    stonePixmap.load(":/images/stone.png");
    woodPixmap.load(":/images/wood.png");
    backgroundPixmap.load(":/images/block.png");
    m_gameEngine = new GameEngine(this);
    setGameEngine(m_gameEngine);
//设置游戏引擎

    initUICards();
    //构造暂停按钮
    m_pauseButton = new QPushButton("暂停", this);
    m_pauseButton->setFixedSize(60, 30);
    m_pauseButton->setToolTip("点击暂停游戏或继续游戏");
    connect(m_pauseButton, &QPushButton::clicked, this, &GameWindow::onPauseButtonClicked);
    m_pauseButton->show();
    //调用开始函数准备开始
    if (m_gameEngine) {
        m_gameEngine->startGame();
    }

    setMinimumSize(PALETTE_CARD_W * 3 + PALETTE_PAD * 4 + 70, //
                    m_paletteHeight + TILE_SIZE * 5); //
//绘制开始界面
    if (m_gameEngine) {
        const Map& map = m_gameEngine->getMap();
        int mapPixelWidth = TILE_SIZE * map.getCols();
        int mapPixelHeight = TILE_SIZE * map.getRows();

        if (map.getRows() > 0 && map.getCols() > 0) {
            int minPaletteWidth = PALETTE_CARD_W * static_cast<int>(m_uiCards.size()) + PALETTE_PAD * (static_cast<int>(m_uiCards.size()) + 1);
            int windowWidth = qMax(mapPixelWidth, minPaletteWidth);
            int windowHeight = mapPixelHeight + m_paletteHeight;
            setFixedSize(windowWidth, windowHeight);

        }
    }

    setWindowTitle("塔防游戏");
}

GameWindow::~GameWindow()
{
}

void GameWindow::initUICards() {
    m_paletteHeight = PALETTE_CARD_H + 2 * PALETTE_PAD;
    m_uiCards.clear();
//上面卡牌图片绘制,包括地图的还有上面卡片的
    Tower::CardInfo meleeCard;
    meleeCard.type = Tower::Type::MELEE;
    meleeCard.name = "障碍物";
    meleeCard.cost = 50;
    meleeCard.cardImg.load(":/images/light.png");
    meleeCard.towerImg.load(":/images/light.png");
    m_uiCards.append(meleeCard);

    Tower::CardInfo rangedCard;
    rangedCard.type = Tower::Type::RANGED;
    rangedCard.name = "远程兵";
    rangedCard.cost = 100;
    rangedCard.cardImg.load(":/images/bottle.png");
    rangedCard.towerImg.load(":/images/bottle.png");
    m_uiCards.append(rangedCard);

}

void GameWindow::setGameEngine(GameEngine* newEngine) {
    //如果有旧的，那么先删除
    if (m_gameEngine) {
        disconnect(m_gameEngine,
                    &GameEngine::gameUpdated,
                    this,
                    static_cast<void (QWidget::*)()>(&QWidget::update)
                   );
        disconnect(m_gameEngine, &GameEngine::gameOverSignal, this, &GameWindow::onGameOver);
    }
//删除旧的之后构造新的
    m_gameEngine = newEngine;
    if (m_gameEngine) {
        connect(m_gameEngine,
                 &GameEngine::gameUpdated,
                 this,
                 static_cast<void (QWidget::*)()>(&QWidget::update)
                );
        connect(m_gameEngine, &GameEngine::gameOverSignal, this, &GameWindow::onGameOver);
        update();
    }
}


void GameWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_paletteRect.setRect(0, 0, width(), m_paletteHeight);
    if (m_pauseButton) {
        int buttonMargin = 5;
        m_pauseButton->move(width() - m_pauseButton->width() - buttonMargin, buttonMargin);
    }
    update();
}
//绘制整个屏幕的逻辑
void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //绘制卡牌
    if (m_paletteRect.isValid()) {
        painter.fillRect(m_paletteRect, QColor(60, 60, 60, 230));
        int currentX = m_paletteRect.left() + PALETTE_PAD;
        for (const auto& card : m_uiCards) {
            QRect cardDrawRect(currentX, m_paletteRect.top() + PALETTE_PAD, PALETTE_CARD_W, PALETTE_CARD_H);
            if (cardDrawRect.right() > m_paletteRect.right() - PALETTE_PAD) break;

            painter.fillRect(cardDrawRect, QColor(90, 90, 90));
            painter.setPen(QColor(150, 150, 150));
            painter.drawRect(cardDrawRect);

            if (!card.cardImg.isNull()) {
                QPixmap scaledImg = card.cardImg.scaled(PALETTE_CARD_W - 8, PALETTE_CARD_H - 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(cardDrawRect.left() + (PALETTE_CARD_W - scaledImg.width()) / 2, cardDrawRect.top() + 4, scaledImg);
            }
            painter.setPen(Qt::yellow);
            painter.setFont(QFont("Arial", 9, QFont::Bold));
            painter.drawText(QRect(cardDrawRect.left(), cardDrawRect.bottom() - 20, PALETTE_CARD_W, 18), Qt::AlignCenter, QString::number(card.cost));

            if (m_gameEngine && m_gameEngine->getPlayerMoney() < card.cost) {
                painter.fillRect(cardDrawRect, QColor(0, 0, 0, 120)); // 金钱不足则置灰
            }
            currentX += PALETTE_CARD_W + PALETTE_PAD;
        }
    }

    int mapOffsetY = m_paletteHeight;
    if (!m_gameEngine || m_gameEngine->getMap().getRows() == 0) {
        QRect mapDrawArea = rect().adjusted(0, mapOffsetY, 0, 0);
        painter.fillRect(mapDrawArea, Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(mapDrawArea, Qt::AlignCenter, "Map not loaded or Engine not set.");
        return;
    }
//绘制地图
    const Map& map = m_gameEngine->getMap();
    for (int r = 0; r < map.getRows(); ++r) {
        for (int c = 0; c < map.getCols(); ++c) {
            const Cell* cell = map.getCell(r, c);
            if (!cell) continue;

            QRectF cellRect(c * TILE_SIZE, r * TILE_SIZE + mapOffsetY, TILE_SIZE, TILE_SIZE);
            QPixmap tileBgPixmap = getPixmapForTileType(static_cast<int>(cell->getType()));

            if (!tileBgPixmap.isNull()) {
                painter.drawPixmap(cellRect, tileBgPixmap, tileBgPixmap.rect());
            } else {
                painter.fillRect(cellRect, QColor(30,30,30));
            }
            if (m_isDragging) {
                QPoint mouseGrid = screenToGrid(QPoint(m_dragMousePos.x(), m_dragMousePos.y() - mapOffsetY));
                if (mouseGrid.x() == c && mouseGrid.y() == r) {
                    if (m_gameEngine->canPlaceTower(m_dragType, r, c)) {
                        painter.fillRect(cellRect, QColor(0, 255, 0, 75)); // 绿色可放置
                    } else {
                        painter.fillRect(cellRect, QColor(255, 0, 0, 75)); // 红色不可放置
                    }
                }
            }
            if (cell->isStartPoint()) { painter.setPen(QPen(Qt::blue, 2)); painter.drawRect(cellRect.adjusted(1,1,-1,-1)); }
            else if (cell->isEndPoint()) { painter.setPen(QPen(Qt::red, 2)); painter.drawRect(cellRect.adjusted(1,1,-1,-1)); }
        }
    }

    const auto& towers = m_gameEngine->getTowers();
    for (const Tower* tower : towers) {
        if (tower) {
            const QPixmap& towerImg = tower->getImage();
            if (!towerImg.isNull()) {
                qreal imgW = towerImg.width(); qreal imgH = towerImg.height();
                QPixmap scaledTowerImg = towerImg.scaled(TILE_SIZE, TILE_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                imgW = scaledTowerImg.width();
                imgH = scaledTowerImg.height();

                qreal drawX = tower->getCol() * TILE_SIZE + (TILE_SIZE - imgW) / 2.0;
                qreal drawY = tower->getRow() * TILE_SIZE + mapOffsetY + (TILE_SIZE - imgH) / 2.0;
                painter.drawPixmap(drawX, drawY, scaledTowerImg);
            } else {
                 painter.setBrush(Qt::cyan);
                 painter.drawEllipse(QRectF(tower->getCol() * TILE_SIZE + TILE_SIZE * 0.25,
                                           tower->getRow() * TILE_SIZE + mapOffsetY + TILE_SIZE * 0.25,
                                           TILE_SIZE * 0.5, TILE_SIZE * 0.5));
            }
        }
    }

//绘制敌人
    if (m_gameEngine) {
        const auto& enemies = m_gameEngine->getEnemies();
        for (const Enemy* enemy : enemies) {
            if (enemy && enemy->isAlive()) {

                enemy->draw(&painter, mapOffsetY, TILE_SIZE);
            }
        }
    }
    //绘制子弹
    const auto& bullets = m_gameEngine->getBullets();

    for (const Bullet* bullet : bullets) {
        if (bullet) {
            bullet->draw(&painter, mapOffsetY);
        }
    }

    if (m_isDragging && !m_dragImage.isNull()) {
        QPoint dragIconCenter = m_dragMousePos;
        painter.setOpacity(0.75);
        QPixmap scaledDragImg = m_dragImage.scaled(TILE_SIZE, TILE_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(dragIconCenter.x() - scaledDragImg.width() / 2, dragIconCenter.y() - scaledDragImg.height() / 2, scaledDragImg);
        painter.setOpacity(1.0);

        if (m_dragRange > 0) {
            QPoint gridUnderMouse = screenToGrid(QPoint(m_dragMousePos.x(), m_dragMousePos.y() - mapOffsetY));
            if (gridUnderMouse.x() != -1) {
                qreal cellCenterX = gridUnderMouse.x() * TILE_SIZE + TILE_SIZE / 2.0;
                qreal cellCenterY = gridUnderMouse.y() * TILE_SIZE + mapOffsetY + TILE_SIZE / 2.0;
                qreal radiusPx = m_dragRange * TILE_SIZE;
                painter.setPen(QPen(Qt::white, 1.5, Qt::SolidLine));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(QPointF(cellCenterX, cellCenterY), radiusPx, radiusPx);
            }
        }
    }
    if (m_gameEngine && m_gameEngine->isPaused()) {
        QRect mapDisplayArea = rect().adjusted(0, mapOffsetY, 0, 0);
        painter.fillRect(mapDisplayArea, QColor(0, 0, 0, 100));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(mapDisplayArea, Qt::AlignCenter, "游戏已暂停");
        if(m_pauseButton) m_pauseButton->setText("继续");
    } else {
        if(m_pauseButton) m_pauseButton->setText("暂停");
    }
//绘制右上角生命图标，还有钱数和波数
    if (m_gameEngine && m_pauseButton) {
        int currentMoney = m_gameEngine->getPlayerMoney();
        QString moneyText = "money: " + QString::number(currentMoney);
        QRect moneyRect(
            m_pauseButton->x() - 150 - 10,
            m_pauseButton->y(),
            150,
            m_pauseButton->height()
        );
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(moneyRect, Qt::AlignRight | Qt::AlignVCenter, moneyText);
        int currentHealth = m_gameEngine->getPlayerHealth();
        QString healthText = "生命: " + QString::number(currentHealth) + " / 5";
        QRect healthRect = moneyRect.translated(0, moneyRect.height());
        painter.setPen(Qt::red);
        painter.drawText(healthRect, Qt::AlignRight | Qt::AlignVCenter, healthText);
        int currentWave = m_gameEngine->getCurrentWaveNumber();
        int totalWaves = m_gameEngine->getTotalWaves();
        QString waveText = "波数: " + QString::number(currentWave) + " / " + QString::number(totalWaves);
        QRect waveRect = healthRect.translated(0, healthRect.height());
        painter.setPen(Qt::cyan);
        painter.drawText(waveRect, Qt::AlignRight | Qt::AlignVCenter, waveText);
    }
    if (m_isGameOver) {
        QRect mapDisplayArea = rect().adjusted(0, m_paletteHeight, 0, 0);
        painter.fillRect(mapDisplayArea, QColor(0, 0, 0, 150));
        painter.setPen(Qt::darkRed);
        painter.setFont(QFont("Arial", 40, QFont::Bold));
        painter.drawText(mapDisplayArea, Qt::AlignCenter, "Game Over");
    }
}
//鼠标点击事件
void GameWindow::mousePressEvent(QMouseEvent *event) {
    //左键的时候才会有拖拽逻辑之类
    if (event->button() == Qt::LeftButton) {
        int cardIdx = getCardIndexAt(event->pos());

        if (cardIdx != -1) {
            const Tower::CardInfo& selectedCard = m_uiCards[cardIdx];
            if (m_gameEngine && m_gameEngine->getPlayerMoney() >= selectedCard.cost) {
                m_isDragging = true;
                m_dragType = selectedCard.type;
                m_dragImage = selectedCard.towerImg;
                m_dragMousePos = event->pos();
                m_dragRange = m_gameEngine->getStaticTowerRange(selectedCard.type);
                setCursor(Qt::ClosedHandCursor);
                update();
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_isDragging) {
            m_isDragging = false;
            setCursor(Qt::ArrowCursor);
            update();
            return;
        }
        int mapRelY = event->pos().y() - m_paletteHeight;
        QPoint gridPos = screenToGrid(QPoint(event->pos().x(), mapRelY));

        if (gridPos.x() != -1 && gridPos.y() != -1) {
            m_gameEngine->awakenTower(gridPos.y(), gridPos.x());
        }
    }
    QWidget::mousePressEvent(event);
}
//鼠标移动，拖拽塔的时候需要
void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        m_dragMousePos = event->pos();
        update();
    }
    QWidget::mouseMoveEvent(event);
}
//看是否能释放，主要用于拖拽塔到格子，然后释放
void GameWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);

        int mapRelY = event->pos().y() - m_paletteHeight;
        QPoint gridPos = screenToGrid(QPoint(event->pos().x(), mapRelY));
        if (m_gameEngine && gridPos.x() != -1 && gridPos.y() != -1) {
            bool success = m_gameEngine->deployTower(gridPos.y(), gridPos.x(), m_dragType);
        }
        m_dragRange = 0;
        m_dragImage = QPixmap();
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

QPixmap GameWindow::getPixmapForTileType(int tileTypeInt) const {
    Cell::TileType type = static_cast<Cell::TileType>(tileTypeInt);
    switch (type) {
    case Cell::Grass:       return grassPixmap;
    case Cell::PathStone:   return stonePixmap;
    case Cell::Obstacle:    return woodPixmap;
    case Cell::Empty:
    default:                return backgroundPixmap;
    }
}

//像素坐标转换地图网络坐标
QPoint GameWindow::screenToGrid(const QPoint& screenPosInMapArea) const {
    if (TILE_SIZE <= 0) return QPoint(-1, -1);
    int col = screenPosInMapArea.x() / TILE_SIZE;
    int row = screenPosInMapArea.y() / TILE_SIZE;

    if (m_gameEngine) {
        const Map& map = m_gameEngine->getMap();
        if (col >= 0 && col < map.getCols() && row >= 0 && row < map.getRows()) {
            return QPoint(col, row);
        }
    }
    return QPoint(-1, -1);
}
//获取指定窗口下的卡牌索引
int GameWindow::getCardIndexAt(const QPoint& windowPos) const {
    if (!m_paletteRect.isValid() || !m_paletteRect.contains(windowPos)) {
        return -1;
    }

    int relX = windowPos.x() - m_paletteRect.left();
    int relY = windowPos.y() - m_paletteRect.top();

    int currentCardX = PALETTE_PAD;
    for (int i = 0; i < m_uiCards.size(); ++i) {
        QRect cardAreaInPalette(currentCardX, PALETTE_PAD, PALETTE_CARD_W, PALETTE_CARD_H);
        if (cardAreaInPalette.contains(relX, relY)) {
            return i;
        }
        currentCardX += PALETTE_CARD_W + PALETTE_PAD;
        if (currentCardX > m_paletteRect.width() - PALETTE_PAD) break;
    }
    return -1;
}
//暂停游戏的暂停逻辑
void GameWindow::onPauseButtonClicked() {
    if (!m_gameEngine) return;

    if (m_gameEngine->isPaused()) {

        resumeGameFromDialog();
    } else if (m_gameEngine->isActive()) {
        m_gameEngine->pauseGame();

        update();
        QMessageBox pauseDialog(this);
        pauseDialog.setWindowTitle("游戏暂停");
        pauseDialog.setText("游戏已暂停。");
        pauseDialog.setIcon(QMessageBox::Information);


        QPushButton *pResumeButton = pauseDialog.addButton("继续游戏", QMessageBox::AcceptRole); // AcceptRole 通常关闭对话框并返回 QDialog::Accepted
        QPushButton *pExitButton = pauseDialog.addButton("退出本局", QMessageBox::RejectRole); // RejectRole 通常关闭对话框并返回 QDialog::Rejected

        pauseDialog.setDefaultButton(pResumeButton);
        pauseDialog.exec();
        if (pauseDialog.clickedButton() == pResumeButton) {
            resumeGameFromDialog();
        } else if (pauseDialog.clickedButton() == pExitButton) {
            exitGameFromDialog();
        }
    }

}

//继续游戏
void GameWindow::resumeGameFromDialog() {
    if (m_gameEngine && m_gameEngine->isPaused()) {
        m_gameEngine->resumeGame();
        update();
    }
}

void GameWindow::exitGameFromDialog() {
    if (m_gameEngine) {
        m_gameEngine->endGame();
    }

    QApplication::quit();
}
//游戏结束，根据传入的参数判断赢了还是输了
void GameWindow::onGameOver(bool playerWon)
{

    m_isGameOver = true;
    update();


    QMessageBox gameOverDialog(this);
    gameOverDialog.setWindowTitle("游戏结束");
    gameOverDialog.setText(playerWon ? "恭喜你，获得了胜利！" : "很遗憾，你失败了。"); //
    gameOverDialog.setIcon(QMessageBox::Information);


    QPushButton *exitButton = gameOverDialog.addButton("退出游戏", QMessageBox::RejectRole); //

   connect(&gameOverDialog,&QMessageBox::finished,QApplication::instance(),&QApplication::quit);
    gameOverDialog.exec();



}