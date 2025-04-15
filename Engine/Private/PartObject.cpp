#include "PartObject.h"
#include "Shader.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
    PartObject_DESC* desc = static_cast<PartObject_DESC*>(pArg);

    m_pParentTransform = desc->pParentTransform;

    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

HRESULT CPartObject::Bind_WorldMatrix(CShader* pShader, const char* constantName)
{
    return pShader->Bind_Matrix(constantName, &m_WorldMatrix);
}

void CPartObject::Free()
{
    __super::Free();
}
