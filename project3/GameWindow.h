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

    // 设置游戏引擎的接口保持不变
    void setGameEngine(GameEngine* engine);

protected:
    // 事件处理函数的声明保持不变，但实现会被简化
    void paintEvent(QPaintEvent *event) override;//绘制
    void mousePressEvent(QMouseEvent *event) override;//后期鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override;//鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event) override;//鼠标释放
    void resizeEvent(QResizeEvent *event) override;//重构

private:
    // 指向游戏引擎的指针
    GameEngine* m_gameEngine;

    // 绘制地图所需贴图
    QPixmap grassPixmap;
    QPixmap stonePixmap;
    QPixmap woodPixmap;
    QPixmap backgroundPixmap;

    // 地图格子的尺寸（cell）
    const int TILE_SIZE = 40;

    // 根据格子类型获取对应贴图
    QPixmap getPixmapForTileType(int tileType) const;

    // 将屏幕坐标转换为地图网格坐标
    QPoint screenToGrid(const QPoint& screenPosInMapArea) const;
};

#endif // GAMEWINDOW_H
