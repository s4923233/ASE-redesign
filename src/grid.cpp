#include "grid.h"

#include <functional>
#include <algorithm>
#include <cmath>
#include <cassert>

//----------------------------------------------------------------------------------------------------------------------
/// @file grid.cpp
/// @brief implementation files for Grid class
//----------------------------------------------------------------------------------------------------------------------
Grid::Grid()
{

}

//----------------------------------------------------------------------------------------------------------------------
Grid::Grid(float _w,float _h,size_t _nColumns,size_t _nRows)
{
    if(!(_w>0 && _h>0)) throw(std::range_error("Error: Null or Negative Area"));
    if(!(_nColumns>0 && _nRows>0)) throw(std::range_error("Error: Null or Negative Area"));

    m_nColumns = _nColumns;
    m_nRows = _nRows;
    m_size = m_nColumns*m_nRows;

    m_width = _w;
    m_height = _h;
    m_deltaU = m_width/static_cast<float>(m_nColumns);
    m_deltaV = m_height/static_cast<float>(m_nRows);

    //init gridpoints
    for(size_t i = 0; i<=m_nColumns; ++i)
    {
        m_gridpointU.push_back(i*m_deltaU);
    }
    for(size_t i = 0; i<=m_nRows; ++i)
    {
        m_gridpointV.push_back(i*m_deltaV);
    }

    vec2 centre;
    for(float y = m_deltaV/2; y<m_height;y+=m_deltaV)
    {
        for(float x = m_deltaU/2; x<m_width;x+=m_deltaU)
        {
            centre.m_x = x;
            centre.m_y = y;
            m_cellCentres.push_back(centre);
        }
    }

    m_maxVelocity = 0.0f;
    initGrids();
}

//----------------------------------------------------------------------------------------------------------------------
Grid::Grid(const Grid& _other)
{
    this->m_nColumns = _other.m_nColumns;
    this->m_nRows = _other.m_nRows;
    this->m_size = m_nRows*m_nColumns;

    this->m_width = _other.m_width;
    this->m_height = _other.m_height;
    this->m_deltaU = m_width/static_cast<float>(m_nColumns);
    this->m_deltaV = m_height/static_cast<float>(m_nRows);

    //init gridpoints
    for(size_t i = 0; i<=m_nColumns; ++i)
    {
        m_gridpointU.push_back(i*m_deltaU);
    }
    for(size_t i = 0; i<=m_nRows; ++i)
    {
        m_gridpointV.push_back(i*m_deltaV);
    }

    vec2 centre;
    for(float y = m_deltaV/2; y<m_height;y+=m_deltaV)
    {
        for(float x = m_deltaU/2; x<m_width;x+=m_deltaU)
        {
            centre.m_x = x;
            centre.m_y = y;
            m_cellCentres.push_back(centre);
        }
    }
    initGrids();
}

//----------------------------------------------------------------------------------------------------------------------
Grid& Grid::operator=(const Grid& _other)
{
    assert(this != &_other);

    this->m_nColumns = _other.m_nColumns;
    this->m_nRows = _other.m_nRows;
    this->m_size = m_nRows*m_nColumns;

    this->m_width = _other.m_width;
    this->m_height = _other.m_height;
    this->m_deltaU = m_width/static_cast<float>(m_nColumns);
    this->m_deltaV = m_height/static_cast<float>(m_nRows);

    //init gridpoints
    m_gridpointU.clear();
    m_gridpointV.clear();
    for(size_t i = 0; i<=m_nColumns; ++i)
    {
        m_gridpointU.push_back(i*m_deltaU);
    }
    for(size_t i = 0; i<=m_nRows; ++i)
    {
        m_gridpointV.push_back(i*m_deltaV);
    }

    vec2 centre;
    m_cellCentres.clear();
    for(float y = m_deltaV/2; y<m_height;y+=m_deltaV)
    {
        for(float x = m_deltaU/2; x<m_width;x+=m_deltaU)
        {
            centre.m_x = x;
            centre.m_y = y;
            m_cellCentres.push_back(centre);
        }
    }

    initGrids();

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
Grid::~Grid(){}

//----------------------------------------------------------------------------------------------------------------------
//initialises the data structures used by the grid
void Grid::initGrids()
{
    //resize Grids
    m_gridInitialVelocityU.resize(m_size,0.0f);
    m_gridVelocityU.resize(m_size,0.0f);
    m_gridDeltaVelocityU.resize(m_size,0.0f);

    m_gridInitialVelocityV.resize(m_size,0.0f);
    m_gridVelocityV.resize(m_size,0.0f);
    m_gridDeltaVelocityV.resize(m_size,0.0f);

    m_gridPressure.resize(m_size,0.0f);

    m_gridCell.resize(m_size);

    //initialize Cell grid and wire U,V,Pressure pointers to appropriate grids
    size_t cell_index = 0;
    vec2 minUV;
    vec2 maxUV;
    std::array<Grid::magnitude_ptr,7> magnitude;
    std::array<Grid::cell_ptr,4> neighbours;

    for(size_t y = 1; y<m_gridpointV.size();++y)
    {
        for(size_t x = 1; x<m_gridpointU.size(); ++x)
        {
            cell_index = toIndex(x-1,y-1);
            minUV.m_x = m_gridpointU.at(x-1);
            minUV.m_y = m_gridpointV.at(y-1);
            maxUV.m_x = m_gridpointU.at(x);
            maxUV.m_y = m_gridpointV.at(y);

            magnitude.at(0) = &(m_gridInitialVelocityU.at(cell_index));
            magnitude.at(1) = &(m_gridVelocityU.at(cell_index));
            magnitude.at(2) = &(m_gridDeltaVelocityU.at(cell_index));
            magnitude.at(3) = &(m_gridInitialVelocityV.at(cell_index));
            magnitude.at(4) = &(m_gridVelocityV.at(cell_index));
            magnitude.at(5) = &(m_gridDeltaVelocityV.at(cell_index));
            magnitude.at(6) = &(m_gridPressure.at(cell_index));

            neighbours.at(0) =
                    (y < m_height) ? &(m_gridCell.at(toIndex(x-1,y))) : nullptr;//N

            neighbours.at(1) =
                    (y >= 2) ? &(m_gridCell.at(toIndex(x-1,y-2))) : nullptr;//S

            neighbours.at(2) =
                    (x < m_width) ? &(m_gridCell.at(toIndex(x,y-1))) : nullptr;//E

            neighbours.at(3) =
                    (x >= 2) ? &(m_gridCell.at(toIndex(x-2,y-1))) : nullptr;//W

            //the cell is initialised with the prepared data.
            //The newly created cell will point to the appropriate velocity locations
            //it also know its neighbours
            m_gridCell.at(cell_index) = Cell(cell_index,minUV,maxUV,magnitude,neighbours);
        }
    }

    Grid::iterator cell_it = m_gridCell.begin();
    while(cell_it!=m_gridCell.end())
    {
        cell_it->initNeighbourIndexList();
        cell_it++;
    }

}

//----------------------------------------------------------------------------------------------------------------------
//converts cartesian coordinates to an index
size_t Grid::toIndex(const size_t _x,const size_t _y)
{
    return _y*m_nColumns + _x;
}

//----------------------------------------------------------------------------------------------------------------------
//convert an index to cartesian coordinates
Grid::vec2 Grid::toCartesian(const size_t _index)
{
    vec2 coordinate;

    coordinate.m_x = static_cast<size_t>(_index%m_nColumns);
    coordinate.m_y = static_cast<size_t>(_index/m_nColumns);

    return coordinate;
}

//----------------------------------------------------------------------------------------------------------------------
//returns the velocity at the specified spatial location
Grid::vec2 Grid::velocity(const vec2 _point)
{
    //Boundaries check
    if ((_point.m_x<0)||(_point.m_x>m_width))
        return vec2(0.0f,0.0f);//throw(std::range_error("Error: Point outside grid boundaries"));
    if ((_point.m_y<0)||(_point.m_y>m_height))
        return vec2(0.0f,0.0f);//throw(std::range_error("Error: Point outside grid boundaries"));

    Cell c = cell(_point); //cell method
    return c.velocity(_point);
}

//----------------------------------------------------------------------------------------------------------------------
//returns the velocity measured at the centre of the cell
Grid::vec2 Grid::velocity(const size_t _x,const size_t _y)
{
    Cell c = m_gridCell[toIndex(_x,_y)];
    return c.velocity(c.centre());
}

//----------------------------------------------------------------------------------------------------------------------
//returns the delta velocity at the specified spatial location
Grid::vec2 Grid::deltaVelocity(const vec2 _point)
{
    //Boundaries check
    if ((_point.m_x<0)||(_point.m_x>m_width))
        return vec2(0.0f,0.0f);//throw(std::range_error("Error: Point outside grid boundaries"));
    if ((_point.m_y<0)||(_point.m_y>m_height))
        return vec2(0.0f,0.0f);//throw(std::range_error("Error: Point outside grid boundaries"));

    Cell c = cell(_point); //cell method
    return c.deltaVelocity(_point);
}

//----------------------------------------------------------------------------------------------------------------------
//returns the delta velocity at the specified cell
Grid::vec2 Grid::deltaVelocity(const size_t _x,const size_t _y)
{
    Cell c = m_gridCell[toIndex(_x,_y)];
    return c.deltaVelocity(c.centre());
}

//----------------------------------------------------------------------------------------------------------------------
//returns the velocity divergence at the specified cell
float Grid::velocityDivergence(const size_t _x,const size_t _y)
{
    Cell c = m_gridCell[toIndex(_x,_y)];
    return c.divergence();
}

//----------------------------------------------------------------------------------------------------------------------
//calculates the max velocity in the system
void Grid::maxVelocityUpdate()
{
    std::vector<float> velocityUSquared(m_size,0.0f);
    std::vector<float> velocityVSquared(m_size,0.0f);
    std::vector<float> velocityUVSum(m_size,0.0f);

    std::transform(m_gridVelocityU.begin(),m_gridVelocityU.end(),
                   m_gridVelocityU.begin(),velocityUSquared.begin(),std::multiplies<float>());

    std::transform(m_gridVelocityV.begin(),m_gridVelocityV.end(),
                   m_gridVelocityV.begin(),velocityVSquared.begin(),std::multiplies<float>());

    std::transform(velocityUSquared.begin(),velocityUSquared.end(),
                   velocityVSquared.begin(),velocityUVSum.begin(),std::plus<float>());

    std::vector<float>::iterator max;
    max = std::max_element(velocityUVSum.begin(),velocityUVSum.end());

    m_maxVelocity = std::sqrt(*max);
}

//----------------------------------------------------------------------------------------------------------------------
//calculates the delta velocity for all the cells.
//deltaVelocity = InitialVelocity-UpdatedVelocity
void Grid::deltaVelocityUpdate()
{
    m_gridDeltaVelocityU.clear();
    std::transform(m_gridVelocityU.begin(),m_gridVelocityU.end(),
                   m_gridInitialVelocityU.begin(),m_gridDeltaVelocityU.begin(),std::minus<float>());

    m_gridDeltaVelocityV.clear();
    std::transform(m_gridVelocityV.begin(),m_gridVelocityV.end(),
                   m_gridInitialVelocityV.begin(),m_gridDeltaVelocityV.begin(),std::minus<float>());

}

//----------------------------------------------------------------------------------------------------------------------
//set the initial velocity to zero
void Grid::resetInitialVelocity()
{
    std::fill(m_gridInitialVelocityU.begin(),m_gridInitialVelocityU.end(),0.0f);
    std::fill(m_gridInitialVelocityV.begin(),m_gridInitialVelocityV.end(),0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
float Grid::density(const size_t _x,const size_t _y)
{
    Cell c = m_gridCell[toIndex(_x,_y)];
    return c.density();
}

//----------------------------------------------------------------------------------------------------------------------
void Grid::pressure(const size_t _index, const float _pressure)
{
    m_gridCell[_index].setPressure(_pressure);
}

//----------------------------------------------------------------------------------------------------------------------
Cell& Grid::cell(const size_t _index)
{
    return m_gridCell[_index];
}

//----------------------------------------------------------------------------------------------------------------------
Cell& Grid::cell(const vec2 _point)
{
    size_t x = static_cast<size_t>(_point.m_x/m_deltaU);
    size_t y = static_cast<size_t>(_point.m_y/m_deltaV);

    return m_gridCell[toIndex(x,y)];
}

//----------------------------------------------------------------------------------------------------------------------
Cell& Grid::cell(const size_t _x,const size_t _y)
{
    return m_gridCell[toIndex(_x,_y)];
}

//----------------------------------------------------------------------------------------------------------------------
//returns the i-th column of cells
std::vector<Grid::cell_ptr> Grid::column(const size_t _index)
{
    if(!(_index<m_nColumns))
        throw(std::out_of_range("Error: Index Out of Range"));

    std::vector<cell_ptr> outColumn;
    for(size_t i = _index; i<m_size; i+=m_nColumns)
    {
        outColumn.push_back(&(m_gridCell.at(i)));
    }

    return outColumn;
}

//----------------------------------------------------------------------------------------------------------------------
//returns the i-th row of cells
std::vector<Grid::cell_ptr> Grid::row(const size_t _index)
{
    if(!(_index<m_nRows))
        throw(std::out_of_range("Error: Index Out of Range"));

    std::vector<cell_ptr> outRow;
    for(size_t i = toIndex(0,_index); i<toIndex(0,_index+1); ++i)
    {
        outRow.push_back((&m_gridCell.at(i)));
    }

    return outRow;
}
