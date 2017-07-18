#ifndef GRID_H
#define GRID_H

#include <ngl/Vec2.h>
#include "cell.h"

class Grid
{
    typedef float* magnitude_ptr;
    typedef Cell* cell_ptr;
    typedef ngl::Vec2 vec2;

private:
    void initGrids();

public:
    Grid();
    Grid(float _w,float _h,size_t _nColumns,size_t _nRows);

    Grid(const Grid& _other);
    Grid& operator=(const Grid& _other);
    ~Grid();


    size_t nColumns() const              {return m_nColumns;}
    size_t nRows() const                 {return m_nRows;}
    size_t size() const                  {return m_size;}
    float width() const                  {return m_width;}
    float height() const                 {return m_height;}
    float deltaU() const                 {return m_deltaU;}
    float deltaV() const                 {return m_deltaV;}
    float maxVelocity() const            {return m_maxVelocity;}


    size_t toIndex(const size_t _x,const size_t _y);
    vec2 toCartesian(const size_t _index);

    vec2 velocity(const vec2 _point);
    vec2 velocity(const size_t _x,const size_t _y);

    float velocityDivergence(const vec2 _point);
    float velocityDivergence(const size_t _x,const size_t _y);

    float density(const vec2 _point);
    float density(const size_t _x,const size_t _y);

    float pressure(const vec2 _point);
    float pressure(const size_t _x,const size_t _y);

    Cell& cell(const size_t _index);
    Cell& cell(const vec2 _point);
    Cell& cell(const size_t _x,const size_t _y);


    std::vector<cell_ptr>column(const size_t _index);
    std::vector<cell_ptr>row(const size_t _index);

    //iterators over the cells
    typedef std::vector<Cell>::iterator iterator;
    typedef std::vector<Cell>::const_iterator const_iterator;

    iterator begin() {return m_gridCell.begin();}
    iterator end() {return m_gridCell.end();}

    const_iterator begin() const {return m_gridCell.begin();}
    const_iterator end() const {return m_gridCell.end();}

    const_iterator cbegin() const {return m_gridCell.cbegin();}
    const_iterator cend() const {return m_gridCell.cend();}

protected:

    void updateObserverList();

    void maxVelocityUpdate();
    void deltaVelocityUpdate();

private:

    size_t m_nColumns;
    size_t m_nRows;
    size_t m_size;

    float m_width;
    float m_height;
    float m_deltaU;
    float m_deltaV;

    std::vector<float> m_gridpointU;
    std::vector<float> m_gridpointV;
    std::vector<vec2> m_cellCentres;


    float m_maxVelocity;

    //three vectors for each velocity direction
    std::vector<float> m_gridInitialVelocityU; //stores the values that come from the particles
    std::vector<float> m_gridVelocityU; //stores the new values, updated after pressure calculation
    std::vector<float> m_gridDeltaVelocityU; //delta value to add to particles velocity

    std::vector<float> m_gridInitialVelocityV;
    std::vector<float> m_gridVelocityV;
    std::vector<float> m_gridDeltaVelocityV;

    std::vector<float> m_gridPressure;
    std::vector<Cell> m_gridCell;
};

#endif // GRID_H




