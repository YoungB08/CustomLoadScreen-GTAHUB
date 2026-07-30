#include "CVehicle.h"
#include "CPed.h"
#include "CCamera.h"
#include "methods.h"

static CVector m_vecGravity = {0.0, 0.0, -1.0};

void CVehicle::setGravityVec(RwV3D vec)
{
    if (driver != LOCAL_PLAYER)
        return;

    CCamera *camera = (CCamera*)CLASS_CCamera;
    CCam* pCam = &camera->Cams[camera->ActiveCam];

    CMatrix matOld, matNew;
    GetMatrixForGravity ( m_vecGravity, matOld );
    GetMatrixForGravity ( vec, matNew );

    CVector* pvecPosition = &matrix->pos;

    matOld.invert ();
    pCam->m_aTargetHistoryPos [ 0 ] = matOld * (pCam->m_aTargetHistoryPos [ 0 ] - *pvecPosition);
    pCam->m_aTargetHistoryPos [ 0 ] = matNew * pCam->m_aTargetHistoryPos [ 0 ] + *pvecPosition;

    pCam->m_aTargetHistoryPos [ 1 ] = matOld * (pCam->m_aTargetHistoryPos [ 1 ] - *pvecPosition);
    pCam->m_aTargetHistoryPos [ 1 ] = matNew * pCam->m_aTargetHistoryPos [ 1 ] + *pvecPosition;

    m_vecGravity = vec;
}
