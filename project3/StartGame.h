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
    //两个按钮，开始和退出
    QVBoxLayout *layout;
    //调整按钮排版
    QPixmap background;
    //游戏开始界面背景
protected:
    void paintEvent(QPaintEvent *event) override;
//绘制窗口

};
#endif // STARTGAME_H
