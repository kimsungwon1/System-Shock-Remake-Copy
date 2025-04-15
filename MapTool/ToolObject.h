#pragma once
#include "GameObject.h"

class CImguiDesign;

BEGIN(Engine)

class CModel;
class CTransform;
class CShader;
class CTexture;
class CCollider;

END

class CToolObject abstract :
    public CGameObject
{
public:
	struct TOOLOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC {
		_bool bPlaced = { false };
		AllObjectType eType = {};
		_wstring szModelName = {};
	};

	static const _wstring TypeToLayername[(_int)AllObjectType::TYPE_END];

protected:
	CToolObject(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* szModelCompName, AllObjectType eType);
	CToolObject(const CToolObject& Prototype);
	~CToolObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool IsPicking(_float3* pOut) override;
public:
	HRESULT SaveObject(ofstream& saveStream) const override;
	HRESULT LoadObject(ifstream& loadStream) override;
public:
	const _wstring& GetPrototypeName() const { return m_szPrototypeName; }
	const _wstring& GetLayerTag() const { return m_szLayerTag; }
	const _wstring& GetModelName() const { return m_szModelCompName; }
	
	_bool IsPlaced() const { return m_bPlaced; }
	void SetPlaced(_bool bPlaced) { m_bPlaced = bPlaced; }

	_float GetLength() const;

	vector<string> Get_AnimationNames() const;
	vector<string> Get_AnimationNames_by50(_uint iOffset) const;
	size_t Get_NumAnimation() const;

	void SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false);

	void Bind_BrushPos(const _float4* pPos, _bool bBrushIt) { m_vBrushPos = *pPos; m_bBrushIt = bBrushIt; }

	virtual void Set_Data(_int iData) {}
	virtual _int Get_Data() const { return 0; }
protected:
	CImguiDesign* m_pGui = nullptr;
	CModel* m_pModelCom = nullptr;
	CShader* m_pShaderCom = nullptr;

	CTexture* m_pBrushTextureCom = nullptr;

	CCollider* m_pColliderCom = nullptr;

	_float4 m_vBrushPos = {};
	_bool m_bBrushIt = false;
protected:
	_wstring m_szPrototypeName = { L"" };
	_wstring m_szLayerTag = { L"" };
	_wstring m_szModelCompName = { L"" };
	_bool m_bPlaced = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

