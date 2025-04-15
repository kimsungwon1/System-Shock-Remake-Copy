#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CInteractives abstract : public CGameObject
{

protected:                                                                                                                              
	CInteractives(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractives(const CInteractives& Prototype);
	virtual ~CInteractives() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool IsPicking(_float3* pOut);
protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END
