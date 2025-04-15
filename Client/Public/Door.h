#pragma once
#include "TileObject.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CDoor : public CTileObject
{
private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& Prototype);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool IsPicking(_float3* pOut) override;
	virtual _bool LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _float* pfDistance) const override;
private:
	HRESULT Ready_Components();
	
	_int m_iData = { 0 };

	_float2 m_vTime = {0.f, 0.5f};
	list<_float3> m_listElec = {};

	_float2 m_vFadeTime = { 0.8f,1.f };

	_float3 m_ArrPoints[20] = { {0.f, 0.f, 0.f}, };
	_int m_iArrNum = {};

	_float3 m_vPoints[4] = {
		{-1.f, 2.f, 0.f}, {1.f,2.f,0.f},
		{1.f,0.f,0.f}, {-1.f,0.f,0.f}
	};

	CGameObject* m_pUI_Pickable = nullptr;
	//CGameObject* m_pUI_NeedKey = nullptr;

	_bool m_bFadeout = false;

	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END