#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fluidsimulator.h"
#include "view.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class MainWindow
/// @brief The MainWindow class is the Control component in the MVC design pattern.
/// The class connects the GUI with the simulation model and the display(view) class.
/// It also controls the simulation execution and the visualised data.
///
/// Originally based on class MainWindow from https://github.com/NCCA/QtNGL.git
///  @author Federico Leone
///  @version 3.0
///  @date
/// @todo Add more option to setup the initial state of the fluid simulation.
//----------------------------------------------------------------------------------------------------------------------

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
    void togglePressureSolver(bool _mode);
    void togglePlayStop();
    void nextFrame();

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
