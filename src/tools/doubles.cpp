//
// doubles.cpp
//
// Created by Daniel Kozitza
//
#include <cmath>
//#include <limits>
#include "../tools.hpp"

//std::numeric_limits<double>::epsilon();

bool tools::equal(double const &d1, double const &d2, double epsilon) {
       return std::fabs(d1 - d2) < epsilon;
}
