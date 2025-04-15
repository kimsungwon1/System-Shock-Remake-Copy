#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Screw;
END

BEGIN(Client)

class CParticle_BossScrew :
    public CGameObject
{
private:
	CParticle_BossScrew(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_BossScrew(const CParticle_BossScrew& Prototype);
	virtual ~CParticle_BossScrew() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Screw* m_pVIBufferCom = { nullptr };

private:
	_int m_iDamage = { 0 };

	_float								m_fFrame = { 0.f };
	_float2 m_vDamageSchedule = { 0.f, 0.2f };

	const class CCortexReaver* m_pOwner = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CParticle_BossScrew* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
