#include "Client_Defines.h"
#include "ToolObject.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Animation.h"

#include "Collider.h"
#include "GameInstance.h"

const _wstring CToolObject::TypeToLayername[(_int)AllObjectType::TYPE_END] = {
        TEXT("Tile"),
        TEXT("Unit"),
        TEXT("Deco"),
        TEXT("Item"),
        TEXT("Interactive"),
        TEXT("Light")
};

CToolObject::CToolObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* szModelCompName, AllObjectType eType)
    : CGameObject(pDevice, pContext)
    , m_pGui(pGui)
    , m_szPrototypeName(szPrototypeName)
    , m_szLayerTag(szLayerTag)
    , m_szModelCompName(szModelCompName)
{
    m_eObjType = eType;
}

CToolObject::CToolObject(const CToolObject& Prototype)
    : CGameObject(Prototype)
    , m_pGui(Prototype.m_pGui)
    , m_szPrototypeName(Prototype.m_szPrototypeName)
    , m_szLayerTag(Prototype.m_szLayerTag)
    , m_szModelCompName(Prototype.m_szModelCompName)
{
}

HRESULT CToolObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CToolObject::Initialize(void* pArg)
{
    TOOLOBJECT_DESC* pDesc = static_cast<TOOLOBJECT_DESC*>(pArg);
    
    if (pDesc->szModelName != TEXT("")) {
        m_szModelCompName = pDesc->szModelName;
    }
    
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }
    if (FAILED(Add_Component(LEVEL_STATIC, m_szModelCompName, TEXT("Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if(pDesc->eType == AllObjectType::TYPE_UNIT)
    {
        if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Shader"),
            reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }
    else
    {
        if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Shader"),
            reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pBrushTextureCom))))
        return E_FAIL;

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};
    aabbDesc.vExtents = { 0.5f, 1.0f, 0.5f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc)))
        return E_FAIL;

    //m_pTransformCom->SetWorldMat(pDesc->transMat);
    m_bPlaced = pDesc->bPlaced;
    m_eObjType = pDesc->eType;

    m_szLayerTag = TypeToLayername[(_int)m_eObjType];

    if(m_pModelCom->GetType() == CModel::TYPE_ANIM)
    {
        m_pModelCom->SetUp_Animation(0, true);
    }

    return S_OK;
}

void CToolObject::Priority_Update(_float fTimeDelta)
{
}

void CToolObject::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    if (m_pModelCom->GetType() == CModel::TYPE_ANIM) {
        if (true == m_pModelCom->Play_Animation(fTimeDelta, nullptr))
        {
            int a = 10;
        }
        int a = 10;
    }
}

void CToolObject::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CToolObject::Render()
{
    //m_pColliderCom->Render();

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if(m_pModelCom->GetType() == CModel::TYPE_ANIM)
        {
            m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
        }

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        if (m_eObjType == AllObjectType::TYPE_TILE)
        {
            if (FAILED(m_pBrushTextureCom->Bind_ShadeResource(m_pShaderCom, "g_BrushTexture", 0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(m_vBrushPos))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_bBrush", &m_bBrushIt, sizeof(_bool))))
                return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    m_bBrushIt = false;

    return S_OK;
}

_bool CToolObject::IsPicking(_float3* pOut)
{
    return m_pModelCom->IsPicking(m_pTransformCom->GetWorldMat(), pOut);
}

HRESULT CToolObject::SaveObject(ofstream& saveStream) const
{
    __super::SaveObject(saveStream);

    _uint modelNameLength = m_szModelCompName.length();
    saveStream.write(reinterpret_cast<const char*>(&modelNameLength), sizeof(modelNameLength));
    saveStream.write(reinterpret_cast<const char*>(m_szModelCompName.c_str()),
        modelNameLength * sizeof(_tchar));

    return S_OK;
}

HRESULT CToolObject::LoadObject(ifstream& loadStream)
{
    __super::LoadObject(loadStream);

    _uint modelNameLength = {};
    _tchar modelName[MAX_PATH] = {};
    loadStream.read(reinterpret_cast<char*>(&modelNameLength), sizeof(_uint));
    loadStream.read(reinterpret_cast<char*>(modelName), sizeof(_tchar) * modelNameLength);
    m_szModelCompName = modelName;

    return S_OK;
}

_float CToolObject::GetLength() const
{
    if (m_pModelCom == nullptr) {
        return 1.f;
    }

    return m_pModelCom->GetLength();
}

vector<string> CToolObject::Get_AnimationNames() const
{
    if (m_pModelCom == nullptr) {
        return {};
    }
    return m_pModelCom->GetAnimationNames();
}

vector<string> CToolObject::Get_AnimationNames_by50(_uint iOffset) const
{
    if (m_pModelCom == nullptr) {
        return {};
    }
    return m_pModelCom->GetAnimationNames_By50(iOffset);
}

size_t CToolObject::Get_NumAnimation() const
{
    if (m_pModelCom == nullptr) {
        return {};
    }
    return m_pModelCom->GetNumAnimation();
}

void CToolObject::SetUp_Animation(_uint iAnimationIndex, _bool isLoop)
{
    if (m_pModelCom == nullptr) {
        return;
    }
    m_pModelCom->SetUp_Animation(iAnimationIndex, isLoop);
}

void CToolObject::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBrushTextureCom);
    Safe_Release(m_pColliderCom);
}
