#pragma once
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_EnemyGunFire : public CBlendObject
{
private:
	CEffect_EnemyGunFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_EnemyGunFire(const CEffect_EnemyGunFire& Prototype);
	virtual ~CEffect_EnemyGunFire() = default;

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
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float2						m_fFrame = { 0.f, 0.5f };

private:
	HRESULT Ready_Components();

public:
	static CEffect_EnemyGunFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END