#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_GunFire :
    public CBlendObject
{
public:
	struct GUNFIRE_DESC : CGameObject::GAMEOBJECT_DESC {
		const _float4x4* pWorldMatrixOrigin = nullptr;
		_float fAngle = { 0.f };
		_float fOffsets[3] = { 0.f, 0.f, 0.f };
	};

private:
	CEffect_GunFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_GunFire(const CEffect_GunFire& Prototype);
	virtual ~CEffect_GunFire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void SetToSight();
	void SetShirinking();
private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float2						m_fFrame = { 0.f, 1.f };

	_float						m_fTurnAngle = 0.f;

	_float3						m_fOffsetToOrigin = {};
	const _float4x4* m_pOriginWorldTransform = nullptr;
private:
	HRESULT Ready_Components();

public:
	static CEffect_GunFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END