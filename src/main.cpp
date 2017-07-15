#include "mainwindow.h" //CONTROLLER
#include <QApplication>

#include "FluidSimulator.h" //MODEL
#include "viewer.h" //VIEW


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
