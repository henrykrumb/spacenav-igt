#include <cmath>

#include "utils.hpp"

float rad2deg(float rad)
{
    return 180.0f * rad / M_PI;
}

float deg2rad(float deg)
{
    return M_PI * deg / 180.0f;
}