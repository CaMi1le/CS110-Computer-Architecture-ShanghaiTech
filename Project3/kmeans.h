////////////////
// 
// File: kmeans.h
//
//  Defines type `point_t` and `color_t` for K-Means simulation.
//
//  * You may add extra declarations if you need.
//
// Jose @ ShanghaiTech University
//
////////////////

#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <iostream>
#include <iomanip>


/*********************************************************
      Your extra "include"s if needed in this header
 *********************************************************/
#include <emmintrin.h>
#include <immintrin.h>
#include <cstring>
/*********************************************************
                           End
 *********************************************************/


/*
 * Structure of a data point.
 * 
 *   Fields:
 *     x - double, the x coordinate value.
 *     y - double, the y coordinate value.
 *
 *   Member functions:
 *     getX() / getY(): Acquire x or y coordinate.
 *     setXY(xi, yi): Assign a new (x, y) position.
 */
typedef struct point_t
{
    double x;
    double y;

    /* Constructors. */
    point_t() : x(0), y(0) {};
    point_t(double xi, double yi) : x(xi), y(yi) {};
    point_t(const point_t& other)
        : x(other.getX()), y(other.getY())
    { return; }

    /* Acquire x or y coordinate. */
    double getX(void) const { return x; };
    double getY(void) const { return y; };

    /* Assign a new (x, y) position. */
    inline void setXY(double xi, double yi)
    {
        x = xi;
        y = yi;
    }

    /* Overload operators */
    point_t& operator= (const point_t& rhs)
    {
        setXY(rhs.getX(), rhs.getY());
        return *this;
    }

    point_t operator+ (double scale)
    {
        point_t ret;
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_load1_pd(&scale);

        _mm_storeu_pd((double*)&ret, _mm_add_pd(a, b));

        return ret;
    }

    point_t operator- (double scale)
    {
        point_t ret;
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_load1_pd(&scale);

        _mm_storeu_pd((double*)&ret, _mm_sub_pd(a, b));

        return ret;
    }

    point_t operator/ (double scale)
    {
        point_t ret;
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_load1_pd(&scale);

        _mm_storeu_pd((double*)&ret, _mm_div_pd(a, b));

        return ret;
    }

    point_t& operator+= (const point_t& rhs) 
    {
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_loadu_pd((double*)&rhs);

        _mm_storeu_pd((double*)this, _mm_add_pd(a, b));
        return *this;
    }

    point_t& operator-= (const point_t& rhs)
    {
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_loadu_pd((double*)&rhs);

        _mm_storeu_pd((double*)this, _mm_sub_pd(a, b));

        return *this;
    }

    point_t& operator/= (double scale)
    {
        // double pt[2] = {x, y};
        __m128d a = _mm_loadu_pd((double*)this);
        __m128d b = _mm_load1_pd(&scale);

        _mm_storeu_pd((double*)this, _mm_div_pd(a, b));

        return *this;
    }

} point_t;


/* Overload `<<` for prettier printing. */
std::ostream&
operator<< (std::ostream& os, const point_t& p)
{
    os.setf(std::ios::fixed);
    os << std::setw(13) << std::setprecision(8) << p.getX() << ", "
       << std::setw(13) << std::setprecision(8) << p.getY();
    return os;
}

/*
 * Specify `color_t` as unsinged 8-bit integer, which represents
 * the color code of a data point.
 *
 */
typedef uint8_t color_t;

/* Overload `<<` for correct printing. */
std::ostream&
operator<< (std::ostream& os, const color_t& c)
{
    os << (unsigned) c;
    return os;
}

/*
 * Function prototypes.
 *
 */
void kmeans (point_t * const data, point_t * const mean,
             color_t * const coloring, const int pn, const int cn);


/*********************************************************
           Your extra declarations can go here
 *********************************************************/

/*********************************************************
                           End
 *********************************************************/


#endif
