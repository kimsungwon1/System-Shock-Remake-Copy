#include "stdafx.h"

#include "Weapon.h"
#include "ContainObject.h"
#include "Transform.h"
#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CItem { pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
    : CItem { Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Weapon_DESC* desc = static_cast<Weapon_DESC*>(pArg);

    m_pSocketTransform = desc->pSocketTransform;

    return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

void CWeapon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeapon::Late_Update(_float fTimeDelta)
{
    _matrix worldMatrix = {};
    if(m_pSocketTransform != nullptr && m_pParentTransform != nullptr)
    {
        _matrix socketMatrix = XMLoadFloat4x4(m_pSocketTransform);

        for (size_t i = 0; i < 3; i++) {
            socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
        }

        worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * socketMatrix * XMLoadFloat4x4(m_pParentTransform);
    }
    else {
        worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    XMStoreFloat4x4(&m_WorldMatrix, worldMatrix);

    if (m_bOnInven && m_pInfluencer == nullptr) {
        return;
    }
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeapon::Render()
{
#ifdef _DEBUG
    if(m_pInfluencer == nullptr)
        m_pColliderCom->Render();
#endif

    if (FAILED(CPartObject::Bind_WorldMatrix(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        if (FAILED(m_pShader->Begin(0)))
            return E_FAIL;
        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

_int CWeapon::GetInfo(const _int* arrIndices) const
{
    if (arrIndices[0] == INFO_TYPE) {
        return m_eItemType;
    }

    if (arrIndices[0] == INFO_LOADNUM) {
        if (arrIndices[1] == 1)
            return m_iMaxLoadNum;
        return m_iLoadNum;
    }
    return 0;
}

void CWeapon::Load(_int iBullets)
{
    if (iBullets + m_iLoadNum > m_iMaxLoadNum) {
        MSG_BOX(L"Gun Bullets are Wrong!!!");
    }
    m_iLoadNum += iBullets;
}

void CWeapon::Free()
{
    __super::Free();
}
