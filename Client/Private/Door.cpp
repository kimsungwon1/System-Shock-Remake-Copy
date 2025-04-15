#include "stdafx.h"

#include "GameInstance.h"
#include "Card.h"
#include "Door.h"
#include "UI_Info.h"

#include "FPSPlayer.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTileObject{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& Prototype)
    : CTileObject{ Prototype }
{
}

HRESULT CDoor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
    CCard::CARD_DESC* pDesc = static_cast<CCard::CARD_DESC*>(pArg);
    m_iData = pDesc->iData;

    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDoor::Priority_Update(_float fTimeDelta)
{
    if (m_vFadeTime.x <= 0.f) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), this);
    }

    if (m_bFadeout) {
        m_vFadeTime.x -= fTimeDelta;
    }

    m_vTime.x += fTimeDelta;

    for (auto point = m_listElec.begin(); point != m_listElec.end(); ) {
        if (point->x > point->y) {
            point = m_listElec.erase(point);
        }
        else {
            point->x += fTimeDelta;
            _float ffff = point->x / point->y;
            ++point;
        }
    }

    if (m_vTime.x > m_vTime.y) {
        m_vTime.x = 0.f;
        _float f3 = m_pGameInstance->Get_Random(0.f, 1.f);
        m_listElec.push_back({ 0.f, 6.f, f3 });
    }

    m_iArrNum = 0;
    for (auto iter : m_listElec) {
        m_ArrPoints[m_iArrNum] = iter;
        m_iArrNum++;
        if (m_iArrNum == 20)
            break;
    }
}

void CDoor::Update(_float fTimeDelta)
{
    _float3 vIn;
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    _bool bPicking = IsPicking(&vIn);
    _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vIn) - pPlayer->GetTransformCom()->Get_State(CTransform::STATE_POSITION)));

    if (bPicking && m_pUI_Pickable == nullptr && fDistance < 2.f) {
        auto* pUI = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"));
        if(pUI != nullptr)
        {
            m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), pUI);
        }

        CUI_Info::INFO_DESCS desc = {};
        desc.fLifeTime = -1.f; desc.strQuote = TEXT("E - Open");

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), TEXT("Prototype_GameObject_UI_TextBox"), &desc);
        
        m_pUI_Pickable = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"));
    }
    if (!bPicking && m_pUI_Pickable != nullptr || m_pUI_Pickable != nullptr && bPicking && fDistance >= 2.f) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), m_pUI_Pickable);
        m_pUI_Pickable = nullptr;
    }

    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_E) && bPicking) {
        if (fDistance < 2.f) {
            auto* pP = dynamic_cast<CFPSPlayer*>(pPlayer);
            if (m_iData == 0 || pP->IsThisDoorOpenable(m_iData))
                m_bFadeout = true;
            else {
                // 여기다 'Key 몇번 필요'라는 UI뜨게.   
                CUI_Info::INFO_DESCS desc = {};
                desc.fLifeTime = 3.f; desc.strQuote = _wstring(TEXT("Key")) + to_wstring(m_iData) + _wstring(TEXT(" is needed"));

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), TEXT("Prototype_GameObject_UI_TextBox"), &desc);

            }
            if (m_pUI_Pickable != nullptr) {
                m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), m_pUI_Pickable);
            }
        }
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CDoor::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CDoor::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_vTime.x, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFullTime", &m_vTime.y, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrame", &m_iData, sizeof(_int))))
        return E_FAIL;
    /*if (FAILED(m_pShaderCom->Bind_RawValue("g_arrPoints", m_ArrPoints, sizeof(_float3) * 20)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iArrNum", &m_iArrNum, sizeof(_int))))
        return E_FAIL;*/
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fadeTime", &m_vFadeTime.x, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(5)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

_bool CDoor::IsPicking(_float3* pOut)
{
    m_pTransformCom->Set_Scaled(2.f, 2.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);
    m_pGameInstance->Transform_MouseRay_ToLocalSpace(m_pTransformCom->GetWorldMat());

    _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _vector v0 = vPosition - vRight * 0.5f + vUp * 0.5f;
    _vector v1 = vPosition + vRight * 0.5f + vUp * 0.5f;
    _vector v2 = vPosition + vRight * 0.5f - vUp * 0.5f;
    _vector v3 = vPosition - vRight * 0.5f - vUp * 0.5f;
    
    if (m_pGameInstance->isPicked_InWorldSpace(
        v0, v1, v2, pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InWorldSpace(
        v0, v2, v3, pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InWorldSpace(
        v1, v0, v2, pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InWorldSpace(
        v0, v3, v2, pOut
    )) {
        goto Compute_WorldPos;
    }
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);
    m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
    return false;
Compute_WorldPos:
    XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr())));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);
    m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
    return true;
}

_bool CDoor::LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _float* pfDistance) const
{
    _bool isIntersect = false;

    _float3 vWorldPoints[4] = { {-2.f, 4.f, 0.f}, { 2.f, 4.f, 0.f}, {2.f, 0.f, 0.f}, {-2.f, 0.f, 0.f} };
    for (_int i = 0; i < 4; i++) {
        XMStoreFloat3(vWorldPoints + i, XMVector3TransformCoord(XMLoadFloat3(vWorldPoints + i), m_pTransformCom->GetWorldMat_XMMat()));
    }

    if (TriangleTests::Intersects(vWorldRay_Pos, XMVector3Normalize(vWorldRay_Dir),
        XMLoadFloat3(vWorldPoints + 0), XMLoadFloat3(vWorldPoints + 1), XMLoadFloat3(vWorldPoints + 2), *pfDistance)) {
        isIntersect = true;
    }
    else if (TriangleTests::Intersects(vWorldRay_Pos, XMVector3Normalize(vWorldRay_Dir),
        XMLoadFloat3(vWorldPoints + 0), XMLoadFloat3(vWorldPoints + 2), XMLoadFloat3(vWorldPoints + 3), *pfDistance)) {
        isIntersect = true;
    }
    else if (TriangleTests::Intersects(vWorldRay_Pos, XMVector3Normalize(vWorldRay_Dir),
        XMLoadFloat3(vWorldPoints + 1), XMLoadFloat3(vWorldPoints + 0), XMLoadFloat3(vWorldPoints + 2), *pfDistance)) {
        isIntersect = true;
    }
    else if (TriangleTests::Intersects(vWorldRay_Pos, XMVector3Normalize(vWorldRay_Dir),
        XMLoadFloat3(vWorldPoints + 0), XMLoadFloat3(vWorldPoints + 3), XMLoadFloat3(vWorldPoints + 2), *pfDistance)) {
        isIntersect = true;
    }

    return isIntersect;
}

HRESULT CDoor::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Beam"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if(FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Buffer"), 
        reinterpret_cast<CComponent**>(& m_pVIBufferCom))))
        return E_FAIL;

    CBounding_OBB::BOUNDING_OBB_DESC Desc = {};
    Desc.vExtents = { 1.f, 1.f, 0.001f };
    Desc.vCenter = { 0.f, 0.9f, 0.f };
    Desc.vAngles = { 0.f, 0.f, 0.f };

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"),
        reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(m_pColliderCom);

    return S_OK;
}

CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDoor* pInstance = new CDoor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
    CDoor* pInstance = new CDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    m_pGameInstance->Delete_Collider(m_pColliderCom);
    Safe_Release(m_pColliderCom);
}