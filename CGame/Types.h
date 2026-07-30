#ifndef TYPES_H
#define TYPES_H

#include <windows.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef class RwV3D CVector;
typedef class RwV3D VECTOR;
typedef class RwMatrix CMatrix;

struct Quaternion;

class RwV3D
{
public:
    float fX, fY, fZ;

    RwV3D(float X = 0.0f, float Y = 0.0f, float Z = 0.0f);

    void clear();
    void normalize();

    float length();
    float angle();
    float angle(float X, float Y);
    float angle(RwV3D r);

    float DotProduct ( const CVector * param ) const;
    void CrossProduct ( const CVector * param );


    RwV3D operator + ( const RwV3D& vecRight ) const
    {
        return RwV3D ( fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ );
    }
    RwV3D operator - ( const RwV3D& vecRight ) const
    {
        return RwV3D ( fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ );
    }
    RwV3D operator * ( const RwV3D& vecRight ) const
    {
        return RwV3D ( fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ );
    }
    RwV3D operator * ( float fRight ) const
    {
        return RwV3D ( fX * fRight, fY * fRight, fZ * fRight );
    }
    RwV3D operator / ( const RwV3D& vecRight ) const
    {
        return RwV3D ( fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ );
    }
    RwV3D operator / ( float fRight ) const
    {
        return RwV3D ( fX / fRight, fY / fRight, fZ / fRight );
    }
    RwV3D operator - () const
    {
        return RwV3D ( -fX, -fY, -fZ );
    }

    void operator += ( float fRight )
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
    }
    void operator += ( const RwV3D& vecRight )
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
    }
    void operator -= ( float fRight )
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
    }
    void operator -= ( const RwV3D& vecRight )
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
    }
    void operator *= ( float fRight )
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
    }
    void operator *= ( const RwV3D& vecRight )
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
    }
    void operator /= ( float fRight )
    {
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
    }
    void operator /= ( const RwV3D& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator== ( const RwV3D& param ) const
    {
        return ( fX == param.fX && fY == param.fY && fZ == param.fZ );
    }
    bool operator!= ( const RwV3D& param ) const
    {
        return !( fX == param.fX && fY == param.fY && fZ == param.fZ );
    }
};

class RwMatrix
{
public:
    RwV3D	right;
    DWORD	flags;
    RwV3D	up;
    float	pad_u;
    RwV3D	at; // front
    float	pad_a;
    RwV3D	pos;
    float	pad_p;

    RwMatrix();
    RwMatrix(Quaternion q);

    void setQuaternion(Quaternion quater);
    Quaternion quaternion();
    void invert ();
    RwMatrix rotate ( const RwV3D * param, float theta );


    RwMatrix operator+ ( const RwMatrix& other ) const
    {
        RwMatrix matResult;
        matResult.right = right + other.right;
        matResult.at = at + other.at;
        matResult.up    = up    + other.up;
        matResult.pos   = pos   + other.pos;
        return matResult;
    }
    RwMatrix operator- ( const RwMatrix& other ) const
    {
        RwMatrix matResult;
        matResult.right = right - other.right;
        matResult.at = at - other.at;
        matResult.up    = up    - other.up;
        matResult.pos   = pos   - other.pos;
        return matResult;
    }
    RwMatrix operator* ( const RwMatrix& other ) const
    {
        RwMatrix matResult;
        matResult.right = (*this) * other.right;
        matResult.at = (*this) * other.at;
        matResult.up    = (*this) * other.up;
        matResult.pos   = (*this) * other.pos;
        return matResult;
    }
    RwMatrix operator/ ( RwMatrix other ) const
    {
        other.invert ();
        return (*this) * other;
    }
    RwV3D operator* ( const RwV3D& vec ) const
    {
        return CVector (
                    right.fX*vec.fX + at.fX*vec.fY + up.fX*vec.fZ,
                    right.fY*vec.fX + at.fY*vec.fY + up.fY*vec.fZ,
                    right.fZ*vec.fX + at.fZ*vec.fY + up.fZ*vec.fZ
                    );
    }

private:
    void QuaternionToMatrix(Quaternion &quater);
};

struct CompressedVector
{
    WORD wX, wY, wZ;
};

struct Quaternion
{
    float fW, fX, fY, fZ;
};

struct Detachable
{
    RwV3D	position;
    uint8_t __unknown_12[4];
    RwV3D	vector;
    uint8_t __unknown_28[16];
};

#endif // TYPES_H
