//
// Created by 21048 on 25-6-26.
//
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>

#include "GameEngine.h"



class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    // 设置游戏引擎
    void setGameEngine(GameEngine* engine);

protected:
    // 事件处理函数
    void paintEvent(QPaintEvent *event) override;//绘制
    void mousePressEvent(QMouseEvent *event) override;//后期鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override;//鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;//鼠标释放
    void resizeEvent(QResizeEvent *event) override;//重构

    private:
    GameEngine* m_gameEngine;//游戏引擎
    QPixmap grassPixmap;
    QPixmap stonePixmap;
    QPixmap woodPixmap;
    QPixmap backgroundPixmap;
    const int TILE_SIZE = 40;

    QPixmap getPixmapForTileType(int tileType) const;
    QPoint screenToGrid(const QPoint& screenPosInMapArea) const;


    void initUICards(); // 初始化卡牌数据
    int getCardIndexAt(const QPoint& windowPos) const;

    QVector<Tower::CardInfo> m_uiCards; // 存储卡牌信息
    QRect m_paletteRect;                // 卡牌区域
    int m_paletteHeight;                // 卡牌区域高度

    // 拖拽状态变量
    bool m_isDragging;
    Tower::Type m_dragType;
    QPixmap m_dragImage;
    QPoint m_dragMousePos;

    // 卡牌UI常量
    const int PALETTE_CARD_W = 60;
    const int PALETTE_CARD_H = 80;
    const int PALETTE_PAD = 5;
};

#endif // GAMEWINDOW_H