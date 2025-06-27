//
// Created by 21048 on 25-6-26.
//
#include "GameWindow.h"
#include <QPainter>
#include <QDebug>
#include "GameEngine.h"

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
      m_isDragging(false),
      m_dragType(Tower::Type::NONE),
      m_paletteHeight(0)
{
    // 加载地块图片
    grassPixmap.load(":/images/grass.png");
    stonePixmap.load(":/images/stone.png");
    woodPixmap.load(":/images/wood.png");
    backgroundPixmap.load(":/images/block.png");

    // 创建引擎
    m_gameEngine = new GameEngine(this);
    // 连接信号，当引擎数据变化时=刷新窗口
    connect(m_gameEngine, &GameEngine::gameUpdated, this, static_cast<void (QWidget::*)()>(&QWidget::update));

    // 初始化UI卡牌
    initUICards();

    // 根据地图和卡牌区域计算窗口大小
    if (m_gameEngine) {
        const Map& map = m_gameEngine->getMap();
        if (map.getRows() > 0 && map.getCols() > 0) {
            int mapPixelWidth = TILE_SIZE * map.getCols();
            int mapPixelHeight = TILE_SIZE * map.getRows();
            int minPaletteWidth = PALETTE_CARD_W * m_uiCards.size() + PALETTE_PAD * (m_uiCards.size() + 1);

            int windowWidth = qMax(mapPixelWidth, minPaletteWidth);
            // 总高度 = 地图高度 + 卡牌区域高度
            int windowHeight = mapPixelHeight + m_paletteHeight;
            setFixedSize(windowWidth, windowHeight);
        }
    }
    setWindowTitle("塔防");
    setMouseTracking(true);
}

// 初始化UI卡牌数据
void GameWindow::initUICards() {
    m_paletteHeight = PALETTE_CARD_H + 2 * PALETTE_PAD;
    m_uiCards.clear();

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

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 绘制卡牌选择区域
    m_paletteRect.setRect(0, 0, width(), m_paletteHeight);
    painter.fillRect(m_paletteRect, QColor(60, 60, 60, 230));
    int currentX = m_paletteRect.left() + PALETTE_PAD;
    for (const auto& card : m_uiCards) {
        QRect cardDrawRect(currentX, m_paletteRect.top() + PALETTE_PAD, PALETTE_CARD_W, PALETTE_CARD_H);

        painter.drawPixmap(cardDrawRect, card.cardImg); // 绘制卡牌图
        // 设置文字属性
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QRect textRect = cardDrawRect;
        textRect.setTop(cardDrawRect.top() + cardDrawRect.height() * 0.6); // 从卡片高度60%的位置开始
        // 绘制价格，居中对齐
        painter.drawText(textRect, Qt::AlignCenter, QString::number(card.cost));
        // 如果钱不够，给卡牌蒙上一层灰色
        if (m_gameEngine && m_gameEngine->getPlayerMoney() < card.cost) {
            painter.fillRect(cardDrawRect, QColor(0, 0, 0, 120));
        }
        currentX += PALETTE_CARD_W + PALETTE_PAD;
    }

    // 绘制金钱
    if (m_gameEngine) {
        QString moneyText = "金钱: " + QString::number(m_gameEngine->getPlayerMoney());
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(QRect(width() - 150, 5, 140, 20), Qt::AlignRight, moneyText);
    }


    // 2. 绘制地图
    int mapOffsetY = m_paletteHeight;
    const Map& map = m_gameEngine->getMap();
    for (int r = 0; r < map.getRows(); ++r) {
        for (int c = 0; c < map.getCols(); ++c) {
            const Cell* cell = map.getCell(r, c);
            QRectF cellRect(c * TILE_SIZE, r * TILE_SIZE + mapOffsetY, TILE_SIZE, TILE_SIZE);
            QPixmap tileBgPixmap = getPixmapForTileType(static_cast<int>(cell->getType()));
            painter.drawPixmap(cellRect, tileBgPixmap, tileBgPixmap.rect());

            // 绘制拖拽时的放置提示 (绿色/红色方块)
            if (m_isDragging) {
                QPoint mouseGrid = screenToGrid(QPoint(m_dragMousePos.x(), m_dragMousePos.y() - mapOffsetY));
                if (mouseGrid.x() == c && mouseGrid.y() == r) {
                    if (m_gameEngine->canPlaceTower(m_dragType, r, c)) {
                        painter.fillRect(cellRect, QColor(0, 255, 0, 75));
                    } else {
                        painter.fillRect(cellRect, QColor(255, 0, 0, 75));
                    }
                }
            }
        }
    }

    // 3. 绘制已经放置的塔
    const auto& towers = m_gameEngine->getTowers();
    for (const Tower* tower : towers) {
        QPixmap scaledTowerImg = tower->getImage().scaled(TILE_SIZE, TILE_SIZE, Qt::KeepAspectRatio);
        qreal drawX = tower->getCol() * TILE_SIZE + (TILE_SIZE - scaledTowerImg.width()) / 2.0;
        qreal drawY = tower->getRow() * TILE_SIZE + mapOffsetY + (TILE_SIZE - scaledTowerImg.height()) / 2.0;
        painter.drawPixmap(drawX, drawY, scaledTowerImg);
    }

    // 4. 绘制正在拖拽的塔的图像
    if (m_isDragging && !m_dragImage.isNull()) {
        painter.setOpacity(0.75);
        QPixmap scaledDragImg = m_dragImage.scaled(TILE_SIZE, TILE_SIZE, Qt::KeepAspectRatio);
        painter.drawPixmap(m_dragMousePos.x() - scaledDragImg.width() / 2, m_dragMousePos.y() - scaledDragImg.height() / 2, scaledDragImg);
        painter.setOpacity(1.0);
    }
}


void GameWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // 检查是否点击在卡牌上
        int cardIdx = getCardIndexAt(event->pos());
        if (cardIdx != -1) {
            const Tower::CardInfo& selectedCard = m_uiCards[cardIdx];
            // 检查钱是否足够
            if (m_gameEngine && m_gameEngine->getPlayerMoney() >= selectedCard.cost) {
                m_isDragging = true;
                m_dragType = selectedCard.type;
                m_dragImage = selectedCard.towerImg;
                m_dragMousePos = event->pos();
                setCursor(Qt::ClosedHandCursor); // 改变鼠标样式
                update();
            }
        }
    }
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        m_dragMousePos = event->pos(); // 更新拖拽位置
        update(); // 触发重绘
    }
}

void GameWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        int mapRelY = event->pos().y() - m_paletteHeight;
        QPoint gridPos = screenToGrid(QPoint(event->pos().x(), mapRelY));

        // 如果释放在有效的地图网格内，则尝试部署
        if (m_gameEngine && gridPos.x() != -1 && gridPos.y() != -1) {
            m_gameEngine->deployTower(gridPos.y(), gridPos.x(), m_dragType);
        }

        // 重置拖拽状态
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void GameWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_paletteRect.setRect(0, 0, width(), m_paletteHeight);
    update();
}

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

int GameWindow::getCardIndexAt(const QPoint& windowPos) const {
    if (!m_paletteRect.contains(windowPos)) {
        return -1;
    }
    int currentCardX = PALETTE_PAD;
    for (int i = 0; i < m_uiCards.size(); ++i) {
        QRect cardArea(currentCardX, PALETTE_PAD, PALETTE_CARD_W, PALETTE_CARD_H);
        if (cardArea.contains(windowPos)) {
            return i;
        }
        currentCardX += PALETTE_CARD_W + PALETTE_PAD;
    }
    return -1;
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
GameWindow::~GameWindow()
{
}