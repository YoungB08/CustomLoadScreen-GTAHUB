#ifndef CPHYSICAL_H
#define CPHYSICAL_H

#include "CGame/Types.h"
#include "CGame/CEntity.h"

class CPhysical : public CEntity
{
public:
    //CEntity entity;
    uint8_t			nImmunities;			// 65
    uint8_t		   __unknown_66;			// 66
    uint8_t				  flags;			/* 66 immunities */
    uint8_t		   __unknown_67;			/* 67 */
    RwV3D				  speed;
    RwV3D				   spin;
    RwV3D		   speed_rammed;
    RwV3D			spin_rammed;
    uint8_t	  __unknown_116[24];			/* 116 */
    float				   mass;
    float		massWhenTurning;
    float			gripDivider;
    // 1.0 = 1 x gGrip
    // 10.1 = 10 x gGrip
    // 100.0 = g / 100Grip
    float			 massToGrip;
    float			 Elasticity;
    float			   Buoyancy;
    RwV3D			 massCenter;
    void		 *__unknown_176;			/* 176 - pointer to a "entry node info" pool item */
    void		 *__unknown_180;			/* 180 - pointer to a "ptr node Double" pool item */
    byte		  __unknown_184;
    WORD		 collisionCount;
    byte		  __unknown_185;
    void  *last_collided_object;
    void  *last_collided_object2;
    void  *last_collided_object3;
    void  *last_collided_object4;
    void  *last_collided_object5;
    void  *last_collided_object6;
    float				 speed_z;			/* 212 */
    float			 damageValue;			/* 216 мб это таймер коллизии */
    CPhysical		  *damagedBy;			/* 220 он может быть CPed'ом */
    RwV3D	  collided_pos_local;
    RwV3D			collided_pos;
    WORD		   collided_part;
    // ѕримечание: ƒл€ автомобилей используютс€ следующие номера частей :
    // 		1 - капот ? (лева€ передн€€ фара ? )
    // 		2 - багажник ?
    // 		3 - передний бампер ?
    // 		4 - задний бампер ?
    // 		5 - лева€ передн€€ дверь
    // 		6 - права€ передн€€ дверь
    // 		7 - лева€ задн€€ дверь
    // 		8 - права€ задн€€ дверь
    // 		9 - левое крыло ?
    // 		10 - правое крыло ?
    // 		17 - права€ передн€€ фара ?
    CPhysical   *attachedSubject;
    RwV3D			  target_pos;
    RwV3D			target_angle;
    byte	   __unknown_280[16];
    CPhysical   *collide_ignored;
    float			  lightLevel;
    DWORD		   __unknown_304;
    DWORD			*CShadowData;

    float getSpeed();
};

#endif // CPHYSICAL_H
