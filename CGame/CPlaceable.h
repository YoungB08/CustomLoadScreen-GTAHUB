#ifndef CPLACEABLE_H
#define CPLACEABLE_H

#include "CGame/Types.h"

class CPlaceable
{
public:
    DWORD    __vmt;   // Указатель на виртуальную таблицу методов для этого объекта
    RwV3D    pos;    // Позиция RwV3D
    float    angle;   // Угол по оси Z
    RwMatrix *matrix; // Указатель на матрицу позиции и вращения
};

#endif // CPLACEABLE_H
