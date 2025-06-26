//
// Created by 21048 on 25-6-26.
//
#include "GameWindow.h"
#include <QPainter>
#include <QDebug>
#include "GameEngine.h"
#include "Cell.h"

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
{

    grassPixmap.load(":/images/grass.png");
    stonePixmap.load(":/images/stone.png");
    woodPixmap.load(":/images/wood.png");
    backgroundPixmap.load(":/images/block.png");

    m_gameEngine = new GameEngine(this);
    setGameEngine(m_gameEngine);

    // 游戏开始
    if (m_gameEngine) {
        m_gameEngine->startGame();
    }

    // 根据地图尺寸设置窗口大小
    if (m_gameEngine) {
        const Map& map = m_gameEngine->getMap();
        int mapPixelWidth = TILE_SIZE * map.getCols();
        int mapPixelHeight = TILE_SIZE * map.getRows();

        if (map.getRows() > 0 && map.getCols() > 0) {
            setFixedSize(mapPixelWidth, mapPixelHeight);
        }
    }
    setWindowTitle("塔防游戏");
}

GameWindow::~GameWindow()
{

}

void GameWindow::setGameEngine(GameEngine* newEngine) {
    // 断开旧引擎的连接,放置奇怪问题
    if (m_gameEngine) {
        disconnect(m_gameEngine, &GameEngine::gameUpdated, this, static_cast<void (QWidget::*)()>(&QWidget::update));
    }

    m_gameEngine = newEngine;

    // 连接新引擎的信号，当游戏状态更新时，重绘窗口
    if (m_gameEngine) {
        connect(m_gameEngine, &GameEngine::gameUpdated, this, static_cast<void (QWidget::*)()>(&QWidget::update));
        update(); // 立即刷新一次界面
    }
}

void GameWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    update();
}

void GameWindow::paintEvent(QPaintEvent *event)//绘制窗口
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int mapOffsetY = 0;
    const Map& map = m_gameEngine->getMap();
    //绘制地图
    for (int r = 0; r < map.getRows(); ++r) {
        for (int c = 0; c < map.getCols(); ++c) {
            const Cell* cell = map.getCell(r, c);
            if (!cell) continue;

            QRectF cellRect(c * TILE_SIZE, r * TILE_SIZE + mapOffsetY, TILE_SIZE, TILE_SIZE);
            QPixmap tileBgPixmap = getPixmapForTileType(static_cast<int>(cell->getType()));

            if (!tileBgPixmap.isNull()) {
                painter.drawPixmap(cellRect, tileBgPixmap, tileBgPixmap.rect());
            }

            // 绘制路径的起点和终点标记
            if (cell->isStartPoint()) {
                painter.setPen(QPen(Qt::blue, 2));
                painter.drawRect(cellRect.adjusted(1,1,-1,-1));
            } else if (cell->isEndPoint()) {
                painter.setPen(QPen(Qt::red, 2));
                painter.drawRect(cellRect.adjusted(1,1,-1,-1));
            }
        }
    }
}

void GameWindow::mousePressEvent(QMouseEvent *event) {
    QPoint gridPos = screenToGrid(event->pos());
    QWidget::mousePressEvent(event);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    // 给后面拖动留位置
    QWidget::mouseMoveEvent(event);
}

void GameWindow::mouseReleaseEvent(QMouseEvent *event) {
   //给后面释放留位置
    QWidget::mouseReleaseEvent(event);
}

// 获取贴图，方便前面访问
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

// 辅助函数：将屏幕坐标转换为网格坐标
QPoint GameWindow::screenToGrid(const QPoint& screenPosInMapArea) const {
    if (TILE_SIZE <= 0) return QPoint(-1, -1);
    int col = screenPosInMapArea.x() / TILE_SIZE;
    int row = screenPosInMapArea.y() / TILE_SIZE;

    if (m_gameEngine) {
        const Map& map = m_gameEngine->getMap();
        if (col >= 0 && col < map.getCols() && row >= 0 && row < map.getRows()) {
            return QPoint(col, row); // QPoint的x是列, y是行
        }
    }
    return QPoint(-1, -1); // 返回无效坐标
}