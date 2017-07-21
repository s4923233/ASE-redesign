#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
/// @file MainWindow.cpp
/// @brief implementation files for MainWindow class
//----------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    this->setFluidSimulator(new FluidSimulator());
    this->setViewer(new View(this, m_fluidsimulator));

    m_ui->s_mainWindowGridLayout->addWidget(m_view,0,0,2,1);

    connect(m_ui->m_displayGrid_CkBox,SIGNAL(toggled(bool)),this,SLOT(toggleGrid(bool)));
    connect(m_ui->m_displayVelocityField_CkBox,SIGNAL(toggled(bool)),this,SLOT(toggleVelocityField(bool)));
    connect(m_ui->m_displayParticles_CkBox,SIGNAL(toggled(bool)),this,SLOT(toggleParticles(bool)));
    connect(m_ui->m_displayActiveCells_CkBox,SIGNAL(toggled(bool)),this,SLOT(toggleActiveCells(bool)));
    connect(m_ui->m_displayBoundaries_CkBox,SIGNAL(toggled(bool)),this,SLOT(toggleBoundaries(bool)));

    connect(m_ui->m_playStop_Btn,SIGNAL(clicked()),this,SLOT(togglePlayStop()));
    connect(m_ui->m_nextFrame_Btn,SIGNAL(clicked()),this,SLOT(nextFrame()));


}

//----------------------------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_fluidsimulator;
    delete m_view;
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setFluidSimulator(FluidSimulator* _fluidsimulator){
    this->m_fluidsimulator = _fluidsimulator;
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setViewer(View* _view){
    this->m_view = _view;
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::toggleGrid(bool _mode)
{
    m_view->setDisplayGrid(_mode);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::toggleVelocityField(bool _mode)
{
    m_view->setDisplayVelocityField(_mode);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::toggleParticles(bool _mode)
{
    m_view->setDisplayParticles(_mode);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::toggleActiveCells(bool _mode)
{
    m_view->setDisplayActiveCells(_mode);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::toggleBoundaries(bool _mode)
{
    m_view->setBoundaries(_mode);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::togglePressureSolver(bool _mode)
{
    m_fluidsimulator->setPressureSolverMode(_mode);
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::togglePlayStop()
{
    m_view->togglePlaySimulation();
}

//----------------------------------------------------------------------------------------------------------------------
void MainWindow::nextFrame()
{
    m_view->togglePlayNextFrame();
}
