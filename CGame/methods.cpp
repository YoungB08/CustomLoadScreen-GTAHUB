#include "methods.h"
#include <cmath>

void GetMatrixForGravity ( CVector& vecGravity, CMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.length() > 0.0001f )
    {
        mat.up = -vecGravity;
        mat.up.normalize ();
        if ( fabs(mat.up.fX) > 0.0001f || fabs(mat.up.fZ) > 0.0001f )
        {
            CVector y ( 0.0f, 1.0f, 0.0f );
            mat.at = vecGravity;
            mat.at.CrossProduct ( &y );
            mat.at.CrossProduct ( &vecGravity );
            mat.at.normalize ();
        }
        else
        {
            mat.at = CVector ( 0.0f, 0.0f, vecGravity.fY );
        }
        mat.right = mat.at;
        mat.right.CrossProduct ( &mat.up );
    }
    else
    {
        // No gravity, use default axes
        mat.right = CVector ( 1.0f, 0.0f, 0.0f );
        mat.at = CVector ( 0.0f, 1.0f, 0.0f );
        mat.up    = CVector ( 0.0f, 0.0f, 1.0f );
    }
}
