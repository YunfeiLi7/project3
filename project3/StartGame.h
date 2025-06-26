//
// Created by 21048 on 25-6-25.
//
#ifndef STARTGAME_H
#define STARTGAME_H
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include "GameWindow.h"
class StartGame:public QWidget{
    Q_OBJECT
public:
    StartGame(QWidget *parent=nullptr);
    ~StartGame();
    signals:
        void gameStartRequested();

    private slots:
        void start();
    void quit();


private:
    QPushButton *Start;
    QPushButton *Quit;
    QVBoxLayout *layout;
    QPixmap background;
    GameWindow *gamewindow;
    //GameEngine *gameengine;
protected:
    void paintEvent(QPaintEvent *event) override;


};
#endif // STARTGAME_H
