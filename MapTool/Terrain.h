#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader*				m_pShaderCom = { nullptr };
	class CTexture*				m_pTextureCom = { nullptr };
	class CTexture*				m_pBrushTextureCom = nullptr;

	class CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };

public:
	virtual _bool IsPicking(_float3* pOut) override;

	void Bind_BrushPos(const _float4* pBrushPos) { m_vBrushPos = *pBrushPos; }

	_float4 m_vBrushPos = {};
private:
	HRESULT Ready_Components();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END