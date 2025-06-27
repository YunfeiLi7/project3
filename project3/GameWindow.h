#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include "Tower.h"
#include "GameEngine.h"
#include <QPushButton>
class GameEngine;

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    void setGameEngine(GameEngine* engine);//与gameengine进行交互
protected:
    void paintEvent(QPaintEvent *event) override;//全部内容的绘制
    void mousePressEvent(QMouseEvent *event) override;//鼠标点击
    void mouseMoveEvent(QMouseEvent *event) override;//鼠标移动
    void mouseReleaseEvent(QMouseEvent *event) override;//鼠标释放
    void resizeEvent(QResizeEvent *event) override;//重塑屏幕大小
    private slots:
        void onPauseButtonClicked();//点击暂停键
    void resumeGameFromDialog();
    void exitGameFromDialog();
    void onGameOver(bool playerWon);

private:
    GameEngine* m_gameEngine;//gamewindow都直接保存一个gameengine
    QPixmap grassPixmap;
    QPixmap stonePixmap;
    QPixmap woodPixmap;
    QPixmap backgroundPixmap;
    const int TILE_SIZE = 40;//图像以及图像大小
    QPixmap getPixmapForTileType(int tileType) const;
    QVector<Tower::CardInfo> m_uiCards;//卡牌的容器，用以绘制
    QRect m_paletteRect;
    bool m_isDragging;//是否拖拽，绘制逻辑
    Tower::Type m_dragType;
    QPixmap m_dragImage;
    QPoint m_dragMousePos;
    int m_dragRange;
//绘制卡牌的一些常量
    const int PALETTE_CARD_W = 60;
    const int PALETTE_CARD_H = 80;
    const int PALETTE_PAD = 5;
    int m_paletteHeight;
//保存一个暂停按钮
    QPushButton* m_pauseButton;
    bool m_isGameOver;
    void initUICards();
    QPoint screenToGrid(const QPoint& screenPosInMapArea) const;//将屏幕像素坐标转换为地图网格坐标
    int getCardIndexAt(const QPoint& windowPos) const;//指定位置卡牌索引
};

#endif // GAMEWINDOW_H