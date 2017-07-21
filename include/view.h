#ifndef VIEW_H
#define VIEW_H
#include <ngl/AbstractVAO.h>
#include <ngl/Vec3.h>
#include <ngl/Camera.h>
#include <ngl/Transformation.h>

#include <QOpenGLWidget>

#include "windowparams.h"
#include "FluidSimulator.h"

//----------------------------------------------------------------------------------------------------------------------
/// @class View
/// @brief The View class is the View component in the MVC design pattern.
/// The class receives the settings from the Control, retreives information from the Model (FluidSimulator class)
/// and displays the data.
/// It is also responsible to initialise the openGL context and assemble and store the geometric data.
///
/// Originally based on class NGLScene from https://github.com/NCCA/SimpleNGL.git
///  @author Federico Leone
///  @version 3.0
///  @date
/// @todo Improve the velocity field visualization to display more intuitive data.
//----------------------------------------------------------------------------------------------------------------------

class View : public QOpenGLWidget
{
    typedef ngl::Vec3 vec3;
public:
    View(QWidget *_parent, FluidSimulator *_fluidSimulator);
    ~View();

    void setFluidSimulator(FluidSimulator* _fluidSimulator);

    void velocityField(const std::vector<vec3> &_data);
    void velocityFieldUpdate(const std::vector<vec3>& _data);

    void activeCells(const std::vector<vec3>& _data);
    void boundaries(const std::vector<vec3>& _data);
    void particles(const std::vector<vec3>& _data);
    void grid();

    void setDisplayGrid(bool _mode)                     {m_displayGrid=_mode;}
    void setDisplayVelocityField(bool _mode)            {m_displayVelocityField=_mode;}
    void setDisplayParticles(bool _mode)                {m_displayParticles=_mode;}
    void setDisplayActiveCells(bool _mode)              {m_displayActiveCells=_mode;}
    void setBoundaries(bool _mode)                      {m_displayBoundaries=_mode;}

    void togglePlaySimulation();
    void togglePlayNextFrame();

protected:
    void resizeGL(int _w, int _h);
    void initializeGL();
    void paintGL();
    void timerEvent(QTimerEvent *);

    void initGridShape(float _w, float _h, size_t _nColumns, size_t _nRows);
    void initCellShape();
    void initParticleShape();
    void initVelocityField(const std::vector<vec3> &_data);

    void updateMVP();
    void resetMVP();

private:
    WinParams m_win;
    ngl::Transformation m_transform;
    ngl::Camera m_camera;

    FluidSimulator* m_fluidSimulator;
    std::unique_ptr<ngl::AbstractVAO> m_velocityFieldVao;
    size_t m_velocityFieldVaoSize;

    size_t m_gridColumns;
    size_t m_gridRows;
    float m_gridWidth;
    float m_gridHeight;
    float m_gridDeltaU;
    float m_gridDeltaV;

    std::vector<vec3> m_cellCentres;

    //display-hide params
    bool m_displayGrid;
    bool m_displayVelocityField;
    bool m_displayParticles;
    bool m_displayActiveCells;
    bool m_displayBoundaries;

    bool m_playSimulation;
    bool m_playNextFrame;

    float m_time;
};

#endif // VIEW_H
