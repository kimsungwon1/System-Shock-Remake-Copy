#pragma once
#include "Interactives.h"
class CRespawnChamber :
    public CInteractives
{
private:
	CRespawnChamber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRespawnChamber(const CRespawnChamber& Prototype);
	virtual ~CRespawnChamber() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;

private:
	HRESULT Ready_Components();

public:
	static CRespawnChamber* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

