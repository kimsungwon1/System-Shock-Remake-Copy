#pragma once
#include "BlendObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

class CEffect_BulletHit : public CBlendObject
{
private:
	CEffect_BulletHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_BulletHit(const CEffect_BulletHit& Prototype);
	virtual ~CEffect_BulletHit() = default;

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
	_float2						m_fFrame = { 0.f, 0.2f };

private:
	HRESULT Ready_Components();

public:
	static CEffect_BulletHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};
