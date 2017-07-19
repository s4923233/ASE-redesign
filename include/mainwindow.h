#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fluidsimulator.h"
#include "view.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void toggleGrid(bool _mode);
    void toggleVelocityField(bool _mode);
    void toggleParticles(bool _mode);
    void toggleActiveCells(bool _mode);
    void toggleBoundaries(bool _mode);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setFluidSimulator(FluidSimulator* _fluidsimulator);
    void setViewer(View* _view);

private:
    Ui::MainWindow *m_ui;
    FluidSimulator *m_fluidsimulator;
    View *m_view;

};

#endif // MAINWINDOW_H
