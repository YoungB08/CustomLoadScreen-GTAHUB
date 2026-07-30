#include "CPed.h"
#include "CVehicle.h"

bool CPed::isDriving()
{
    return state == 50;
}

bool CPed::isDriver()
{
    if (!isDriving())
        return false;
    return vehicle->driver == this;
}

bool CPed::isPassenger()
{
    return (isDriving() && !isDriver());
}

bool CPed::isLeavingCar()
{
    if (isDriving())
        return false;
    return state == 0;
}

bool CPed::isActorDead()
{
    return (state == 55 || state == 63);
}

RwV3D CPed::getBonePos(byte boneId, bool includeAnim)
{
    RwV3D Position = {0.0, 0.0, 0.0};

    typedef int(__thiscall *CPed__getBonePositionWithOffset) (CPed *_this, RwV3D *_offset, int _bodeId, bool _includeAnim);
    (CPed__getBonePositionWithOffset(BONE_POS_FUNC))(this, &Position, boneId, includeAnim);

    return Position;
}
