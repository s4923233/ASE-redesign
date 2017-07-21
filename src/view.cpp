#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>
#include <ngl/ShaderLib.h>

#include <math.h>

#include "view.h"

//----------------------------------------------------------------------------------------------------------------------
/// @file View.cpp
/// @brief implementation files for View class
//----------------------------------------------------------------------------------------------------------------------
View::View(QWidget *_parent, FluidSimulator *_fluidSimulator)
{
    this->resize(_parent->size());
    setFluidSimulator(_fluidSimulator);

    this->m_displayGrid = true;
    this->m_displayVelocityField = false;
    this->m_displayParticles = true;
    this->m_displayActiveCells = false;

}

//----------------------------------------------------------------------------------------------------------------------
View::~View(){}

//----------------------------------------------------------------------------------------------------------------------
void View::setFluidSimulator(FluidSimulator* _fluidSimulator)
{
    this->m_fluidSimulator = _fluidSimulator;
}

//----------------------------------------------------------------------------------------------------------------------
void View::resizeGL(int _w , int _h)
{
    m_win.width  = static_cast<int>( _w * devicePixelRatio() );
    m_win.height = static_cast<int>( _h * devicePixelRatio() );
}

//----------------------------------------------------------------------------------------------------------------------
void View::initializeGL()
{
    ngl::NGLInit::instance();

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    //setup VAOs
    initGridShape(m_fluidSimulator->width(),m_fluidSimulator->height(),
             m_fluidSimulator->nColumns(),m_fluidSimulator->nRows());
    initParticleShape();
    initCellShape();
    initVelocityField(m_fluidSimulator->cellCentres());

    //setup camera
    vec3 from(m_gridWidth/2.0f,m_gridHeight/2.0f,6.0f);
    vec3 to(m_gridWidth/2.0f,m_gridHeight/2.0f,0.0f);
    vec3 up(0.0f,1.0f,0.0f);

    m_camera.set(from,to,up);
    m_camera.setShape(45,float(1024/720),0.1,300);

    //load shader
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->use("nglColourShader");
    shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

    startTimer(20);
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw if active
    if(m_displayGrid)
    {
        grid();
    }

    if(m_displayVelocityField)
    {
        velocityField(m_fluidSimulator->velocityField(m_time));
    }

    if(m_displayParticles)
    {
        particles(m_fluidSimulator->particles());
    }

    if(m_displayActiveCells)
    {
        activeCells(m_fluidSimulator->activeCells(m_time));
    }

    if(m_displayBoundaries)
    {
        boundaries(m_fluidSimulator->boundaries());
    }
}


//----------------------------------------------------------------------------------------------------------------------
void View::timerEvent(QTimerEvent *)
{
    static float time;
    time+=0.1f;
    m_time = time;

    if(m_playSimulation||m_playNextFrame)
    {
        m_fluidSimulator->advanceFrame();
        m_playNextFrame = false;
    }
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void View::activeCells(const std::vector<vec3>& _data)
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    m_transform.setRotation(vec3(90.0f,0.0f,0.0f));
    updateMVP();

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    std::vector<vec3>::const_iterator cellCntr_it = _data.begin();
    while(cellCntr_it != _data.end())
    {
        m_transform.setPosition(*cellCntr_it);
        updateMVP();

        prim->draw("cell");
        cellCntr_it++;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void View::boundaries(const std::vector<vec3>& _data)
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    m_transform.setRotation(vec3(90.0f,0.0f,0.0f));
    updateMVP();

    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    std::vector<vec3>::const_iterator cellCntr_it = _data.begin();
    while(cellCntr_it != _data.end())
    {
        m_transform.setPosition(*cellCntr_it);
        updateMVP();

        prim->draw("cell");
        cellCntr_it++;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void View::particles(const std::vector<vec3>& _data)
{
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    std::vector<vec3>::const_iterator p_it = _data.begin();
    while(p_it != _data.end())
    {
        m_transform.setPosition(*p_it);
        updateMVP();

        prim->draw("sphere");
        p_it++;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void View::grid()
{
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    vec3 simulationCentre(m_gridWidth/2.0f,m_gridHeight/2.0f,0.0f);

    m_transform.setPosition(simulationCentre);
    m_transform.setRotation(vec3(90.0f,0.0f,0.0f));
    updateMVP();

    prim->draw("grid");
}

//----------------------------------------------------------------------------------------------------------------------
void View::initGridShape(float _w, float _h, size_t _nColumns, size_t _nRows)
{
    m_gridColumns  = _nColumns;
    m_gridRows = _nRows;
    m_gridWidth = _w;
    m_gridHeight  = _h;
    m_gridDeltaU = static_cast<float>(_w/_nColumns);
    m_gridDeltaV = static_cast<float>(_h/_nRows);

    //create cell shape
    initCellShape();

    //create grid wireframe
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    prim->createLineGrid("grid",_w,_h, _nColumns);
}

//----------------------------------------------------------------------------------------------------------------------
void View::initCellShape()
{
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    prim->createTrianglePlane("cell",m_gridDeltaU,m_gridDeltaV,1,1,vec3(0.0f,0.0f,1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void View::initParticleShape()
{
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    prim->createSphere("sphere",0.05,4);
}

//----------------------------------------------------------------------------------------------------------------------
void View::initVelocityField(const std::vector<vec3>& _data)
{
    m_cellCentres = _data;

    std::vector<vec3> geometricData;
    vec3 vertex;
    vertex.m_z = 0.0f;

    //V0
    vertex.m_x = 0.0f;
    vertex.m_y = 0.0f;
    geometricData.push_back(vertex);

    //V1
    vertex.m_x = m_gridDeltaU*0.35f;
    geometricData.push_back(vertex);

    m_velocityFieldVaoSize = geometricData.size();
    m_velocityFieldVao.reset(ngl::VAOFactory::createVAO(ngl::simpleVAO,GL_LINES));
    m_velocityFieldVao->bind();
    m_velocityFieldVao->setData(ngl::AbstractVAO::VertexData(geometricData.size()*sizeof(vec3),
                                                geometricData[0].m_x));
    m_velocityFieldVao->setVertexAttributePointer(0,3,GL_FLOAT,0,0);
    m_velocityFieldVao->setNumIndices(geometricData.size());
    m_velocityFieldVao->unbind();
}

//----------------------------------------------------------------------------------------------------------------------
void View::velocityField(const std::vector<vec3>& _data)
{
    resetMVP();
    updateMVP();

    float dot;
    float det;
    float angle;

    vec3 v1;//oriented vector
    vec3 xc(1.0f,0.0f,0.0f);//canonical x-axis
    vec3 zc(0.0f,0.0f,1.0f);//canonical z-axis

    for(size_t i = 0; i<_data.size(); ++i)
    {
        //calculate the angle between x-axis and the oriented input vector
        v1 = _data[i];
        dot = v1.m_x * xc.m_x +
                v1.m_y * xc.m_y +
                v1.m_z * xc.m_z;

        det = v1.m_x*xc.m_y*zc.m_z +
                xc.m_x*zc.m_y*v1.m_x+
                zc.m_x*v1.m_y*xc.m_z-
                v1.m_z*xc.m_y*zc.m_x-
                xc.m_z*zc.m_y*v1.m_x-
                zc.m_z*v1.m_y*xc.m_x;
        angle = atan2(det, dot);

        angle = angle *360.0f/3.14159265f;//convert to degrees

        //update transform matrix
        m_transform.setPosition(m_cellCentres[i]);
        m_transform.setRotation(0.0f,0.0f,angle);
        updateMVP();

        //draw velocity field
        m_velocityFieldVao->bind();
        m_velocityFieldVao->draw();
        m_velocityFieldVao->unbind();
    }

}

//----------------------------------------------------------------------------------------------------------------------
void View::updateMVP()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    shader->use("nglColourShader");

    ngl::Mat4 MV;
    ngl::Mat4 MVP;

    MV=m_transform.getMatrix()*m_camera.getViewMatrix();
    MVP=MV*m_camera.getProjectionMatrix();
    shader->setUniform("MVP",MVP);
}

//----------------------------------------------------------------------------------------------------------------------
void View::resetMVP()
{
    m_transform.setPosition(vec3(0.0f,0.0f,0.0f));
    m_transform.setRotation(vec3(0.0f,0.0f,0.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void View::togglePlaySimulation()
{
    m_playSimulation= !m_playSimulation;
}

//----------------------------------------------------------------------------------------------------------------------
void View::togglePlayNextFrame()
{
    m_playSimulation = false;
    m_playNextFrame = true;
}
