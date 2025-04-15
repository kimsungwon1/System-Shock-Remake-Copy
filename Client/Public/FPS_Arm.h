#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CFPS_Arm :
	public CPartObject
{
public:
	struct Player_Arm_DESC : CPartObject::PartObject_DESC {
		_uint* iParentState = nullptr;
	};

private:
	CFPS_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFPS_Arm(const CFPS_Arm& Prototype);
	virtual ~CFPS_Arm() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
private:
	_bool m_bAnimFinished = true;
	_bool m_bPlayAnimNothing = true;
private:
	HRESULT Ready_Components();
public:
	vector<string> Get_AnimationNames() const;
	vector<string> Get_AnimationNames_by50(_uint iOffset) const;
	vector<string> Get_AnimationNames_10(_uint iNum) const;
	size_t Get_NumAnimation() const;

	_uint Get_iCurrentAnimIndex() const;
	_double Get_CurrentAnimDuration() const;
	_bool IsAnimFinished() const { return m_bAnimFinished; }
	
	_bool IsPlayAnimNothing() const { return m_bPlayAnimNothing; }
	void SetPlayAnimNothing(_bool bDontPlay) { m_bPlayAnimNothing = bDontPlay; }
public:
	void SetUp_Animation(_uint iAnimIndex, _bool bLoop = false);
	void SetUp_Animation_Immediately(_uint iAnimIndex, _bool bLoop = false);
	void Set_Order(_int iOrder) override;

	virtual _int GetInfo(const _int* arrIndices) const override;
public:
	static CFPS_Arm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END