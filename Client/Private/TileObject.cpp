#include "stdafx.h"
#include "TileObject.h"

#include "GameInstance.h"

CTileObject::CTileObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject {pDevice, pContext}
{
    m_eObjType = AllObjectType::TYPE_TILE;
}

CTileObject::CTileObject(const CTileObject& Prototype)
	: CBlendObject{ Prototype }
{
    m_eObjType = AllObjectType::TYPE_TILE;
}

HRESULT CTileObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTileObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Tile_Desc* pDesc = static_cast<Tile_Desc*>(pArg);

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, pDesc->szModelName, TEXT("Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
    m_fFrustumRadius = 3.f;

	return S_OK;
}

void CTileObject::Priority_Update(_float fTimeDelta)
{
}

void CTileObject::Update(_float fTimeDelta)
{
}

void CTileObject::Late_Update(_float fTimeDelta)
{
    if(m_pGameInstance->IsIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTileObject::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        _int iShader = 1;
        
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            iShader = 0;

        if (FAILED(m_pShaderCom->Begin(iShader)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

_bool CTileObject::LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _float* pfDistance) const
{
    return m_pModelCom->LineIntersects(vWorldRay_Pos, vWorldRay_Dir, m_pTransformCom->GetWorldMat_XMMat(), pfDistance);
}

HRESULT CTileObject::Ready_Components()
{
	return S_OK;
}

CTileObject* CTileObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTileObject* pInstance = new CTileObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CTileObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTileObject::Clone(void* pArg)
{
    CTileObject* pInstance = new CTileObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTileObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTileObject::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
