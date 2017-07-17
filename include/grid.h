#ifndef GRID_H
#define GRID_H

#include <ngl/Vec2.h>

class Grid
{
    typedef ngl::Vec2 vec2;
public:
    Grid();
    Grid(float _w,float _h,size_t _nColumns,size_t _nRows);

    float width() const             {return m_width;}

    float height() const              {return m_height;}

    size_t nColumns() const              {return m_nColumns;}

    size_t nRows()const              {return m_nRows;}

private:
    float m_width;
    float m_height;
    size_t m_nColumns;
    size_t m_nRows;
};

#endif // GRID_H
