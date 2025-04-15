#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CTileObject : public CBlendObject
{
public:
	struct Tile_Desc : CGameObject::GAMEOBJECT_DESC {
		_wstring szModelName = {};
	};
protected:
	CTileObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTileObject(const CTileObject& Prototype);
	virtual ~CTileObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _bool LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _float* pfDistance) const;

protected:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

protected:
	HRESULT Ready_Components();

public:
	static CTileObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END