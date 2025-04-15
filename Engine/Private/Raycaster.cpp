#include "Raycaster.h"
#include "GameInstance.h"

CRaycaster::CRaycaster()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CRaycaster::Update()
{
    POINT ptMouse{};
    if(!m_bPickingToCenter)
    {
        GetCursorPos(&ptMouse);

        ScreenToClient(m_hWnd, &ptMouse);
    }
    else {
        ptMouse.x = m_iWinSizeX / 2;
        ptMouse.y = m_iWinSizeY / 2;
    }


    auto projInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);
    auto viewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    _float fProjectionX = 2.f * ptMouse.x / m_iWinSizeX - 1.f;
    _float fProjectionY = -2.f * ptMouse.y / m_iWinSizeY + 1.f;
    _float fProjectionZ = 0.f;

    _vector pointProjection = { fProjectionX, fProjectionY, fProjectionZ, 1.f };

    _vector pointView;
    _vector pointWorld;

    _vector vecWorld;
    //_vector vecLocal;

    pointView = XMVector3TransformCoord(pointProjection, projInverse);
    //pointView.m128_f32[2] = 0.1f;

    pointWorld = XMVector3TransformCoord(pointView, viewInverse);
    vecWorld = XMVector3TransformNormal(pointView, viewInverse);
    vecWorld = XMVector3Normalize(vecWorld);

    XMStoreFloat3(&m_vRayPos, pointWorld);
    XMStoreFloat3(&m_vRayDir, vecWorld);
}

void CRaycaster::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
    _matrix WorldMatrixInverse;
   WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix));

    XMStoreFloat3(&m_vRayPos_Local, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInverse));
    XMStoreFloat3(&m_vRayDir_Local, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInverse));
    XMStoreFloat3(&m_vRayDir_Local, XMVector3Normalize(XMLoadFloat3(&m_vRayDir_Local)));
}

_bool CRaycaster::isPicked_InLocalSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut)
{
    _bool bCasted = false;
    _float fDist = 0.f;
    bCasted = TriangleTests::Intersects(XMLoadFloat3(&m_vRayPos_Local),
        XMLoadFloat3(&m_vRayDir_Local), vPointA, vPointB, vPointC, fDist);
    if (bCasted == false) {
        return false;
    }

    XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPos_Local) + XMLoadFloat3(&m_vRayDir_Local) * fDist);

    return bCasted;
}

_bool CRaycaster::isPicked_InWorldSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut)
{
    _bool bCasted = false;
    _float fDist = 0.f;
    bCasted = TriangleTests::Intersects(XMLoadFloat3(&m_vRayPos),
        XMLoadFloat3(&m_vRayDir), vPointA, vPointB, vPointC, fDist);
    if (bCasted == false) {
        return false;
    }

    XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPos_Local) + XMLoadFloat3(&m_vRayDir_Local) * fDist);

    return bCasted;
}

HRESULT CRaycaster::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

CRaycaster* CRaycaster::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    auto* pInstance = new CRaycaster;
    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY))) {
        MSG_BOX(L"Failed to Create :  CRaycaster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRaycaster::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
