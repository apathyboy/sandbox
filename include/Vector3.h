/**
 * Legacy Sandbox Project - Vector3.h
 *
 * @copyright   Copyright (c) 2009 Eric S. Barr Jr.
 * @author      Eric S. Barr Jr. <eric.barr@ericscottbarr.com>
**/

#ifndef VECTOR3_H_
#define VECTOR3_H_

template<typename T>
class Vector3
{
public:
    Vector3()
        : x_(0)
        , y_(0)
        , z_(0)
    {}

    Vector3(T x, T y, T z)
        : x_(x)
        , y_(y)
        , z_(z)
    {}

    void set(T x, T y, T z)
    {
        x_ = x;
        y_ = y;
        z_ = z;
    }

    T x() { return x_; }
    void x(T x) { x_ = x; }

    T y() { return y_; }
    void y(T y) { y_ = y; }

    T z() { return z_; }
    void z(T z) { z_ = z; }

private:
    T x_;
    T y_;
    T z_;
};

#endif // VECTOR3_H_

