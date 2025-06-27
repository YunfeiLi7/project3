//
// Created by 21048 on 25-6-26.
//
#include "Tower.h"
#include <QDebug>

// 基类构造函数
Tower::Tower(int row, int col) : m_row(row), m_col(col) {}

// 近战塔构造函数，加载自己的图像
MeleeTower::MeleeTower(int row, int col) : Tower(row, col) {
    m_image.load(":/images/light.png");
}

// 远程塔构造函数，加载自己的图像
RangedTower::RangedTower(int row, int col) : Tower(row, col) {
    !m_image.load(":/images/bottle.png");
}