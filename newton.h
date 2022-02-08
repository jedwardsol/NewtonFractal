#pragma once
#include <complex>
#include <boost/multiprecision/cpp_complex.hpp>
#include <type_traits>

constexpr bool FastDisplay{false};

using FastPoint = std::complex<double>;                         // bad at 10-15
using GoodPoint = boost::multiprecision::cpp_complex_quad;
//using GoodPoint = boost::multiprecision::cpp_complex_oct;

using Point     = std::conditional_t<FastDisplay, FastPoint,  GoodPoint>;


Point  fromPixel(int row, int column);
void go(Point const &topLeft,Point const &bottomRight);
void goUp();
void stop();
