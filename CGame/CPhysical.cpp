#include "CPhysical.h"
#include <cmath>

float CPhysical::getSpeed()
{
    return speed.length() * 161.0f;
}
