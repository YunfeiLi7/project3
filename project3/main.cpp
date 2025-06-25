#include <QApplication>
#include <QDebug>
#include "StartGame.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartGame  startGameWindow;
    startGameWindow.show();
    return a.exec();
}
