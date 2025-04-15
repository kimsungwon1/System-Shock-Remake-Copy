#include "Tile.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Shader.h"

CTile::CTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* ModelCompName, AllObjectType eType)
    : CToolObject(pDevice, pContext, pGui, szPrototypeName, szLayerTag, ModelCompName, eType)
{
}

CTile::CTile(const CTile& Prototype)
    : CToolObject(Prototype)
{
}

HRESULT CTile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTile::Initialize(void* pArg)
{
	auto* desc = static_cast<CToolObject::TOOLOBJECT_DESC*>(pArg);
		
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Add_Component(LEVEL_STATIC, desc->modelName/*TEXT("Prototype_Component_Model_Floor1")*/, TEXT("Model"),
	//	reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;


	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Shader"),
	//	reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

    return S_OK;
}

void CTile::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CTile::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}


CTile* CTile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* ModelCompName, AllObjectType eType)
{
	CTile* pInstance = new CTile(pDevice, pContext, pGui, szPrototypeName, szLayerTag, ModelCompName, eType);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTile::Clone(void* pArg)
{
	CTile* pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CTile::Free()
{
	__super::Free();

}
