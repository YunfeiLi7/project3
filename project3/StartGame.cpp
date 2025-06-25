//
// Created by 21048 on 25-6-25.
//
#include "StartGame.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QApplication>
StartGame::StartGame(QWidget *parent) : QWidget(parent), background(":/images/picture.jpg") {

    setWindowTitle("Start Menu");
    resize(400, 300);
    //创造两个按钮
    Start=new QPushButton("start",this);
    Quit=new QPushButton("quit",this);
    //用于排版
    layout =new QVBoxLayout(this);

    layout->addWidget(Start);
    layout->addWidget(Quit);
    layout->setAlignment(Qt::AlignHCenter);
    connect(Start,&QPushButton::clicked,this,&StartGame::start);
    connect(Quit,&QPushButton::clicked,this,&StartGame::quit);
}
void StartGame::start(){
    // this->hide();
    // emit gameStartRequested();
}
void StartGame::quit(){
    QApplication::quit();
}


void StartGame::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(rect(), background);
}

StartGame::~StartGame() {}
