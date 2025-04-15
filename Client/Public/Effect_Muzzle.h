#pragma once
#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

class CEffect_Muzzle : public CBlendObject
{
public:
	struct MUZZLE_DESC : CGameObject::GAMEOBJECT_DESC {
		_float fScale = 1.f;
		CGameObject* pOwner = nullptr;
	};
private:
	CEffect_Muzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Muzzle(const CEffect_Muzzle& Prototype);
	virtual ~CEffect_Muzzle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT make_Flame(_int i);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	//_int						m_iRealFrame = 0;
	_float3						m_fFrame = { 0.f, 0.1f, 0.16f };
	_float2						m_vRandPos[4] = { {0.f, 0.f}, };
	_float						m_fScale = 1.f;
	CGameObject* m_pOwner = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CEffect_Muzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

