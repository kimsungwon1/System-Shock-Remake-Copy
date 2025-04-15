#pragma once

#include "Component.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
	struct MaterialTexturesPath {
		_uint	materialIndex = {};
		_uint	textureType = {};
		_wstring path = {};
	};
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	TYPE GetType() const { return m_eType; }

	_int Get_BoneIndex(const _char* pBoneName) const;

	_matrix Get_BoneCombinedTransformationMatrix(_uint iBoneIndex) const {
		return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
	}
	const _float4x4* Get_BoneCombinedTransformMatrix_Ptr(const _char* pConstantName) const;
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false, _double dAnimToAnimSpeed = 3.0);
	void SetUp_Animation_Immediately(_uint iAnimationIndex, _bool isLoop = false, _double dAnimToAnimSpeed = 3.0);
	// 뼈를 움직인다 - CBone의 m_TransformationMatrix 행렬을 갱신한다.
	// 모든 뼈가 가지고 있는 m_CombinedTransformationMatrix를 갱신한다.
	_bool Play_Animation(_float fTimeDelta, _float4x4* rootBoneMatrix, _bool dontmove = false);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex);
	HRESULT Bind_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

public:
	const string& GetModelName() const { return m_szModelName; }
	const vector<string> GetAnimationNames() const;
	const vector<string> GetAnimationNames_By50(_uint iNum) const;
	const vector<string> GetAnimationNames_10(_uint iNum) const;
	size_t GetNumAnimation() const { return m_Animations.size(); }
	_uint GetCurrentAnimIndex() const { return m_iCurrentAnimIndex; }
	_double GetCurrentAnimDuration() const;
	const _float4x4* GetPreTransformMatrix_Ptr() const { return &m_PreTransformMatrix; }
	_matrix GetPreTransformMatrix() const { return XMLoadFloat4x4(&m_PreTransformMatrix); }

	_bool LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance);
public:
	// 정사각형 타일인걸 가정하고 x,y,z 길이중 하나인 것을 고르기
	_float GetLength() const;

	_float GetSizeX() const;
	_float GetSizeY() const;
	_float GetSizeZ() const;

	_float GetMinXorYorZ(int i) const;
	_float GetMaxXorYorZ(int i) const;
public:
	virtual HRESULT SaveModel(ofstream& saveStream) const;
	virtual HRESULT LoadModel(ifstream& loadStream);

public:
	_bool IsPicking(const _float4x4& WorldMatrix, _float3* pOut);
	
private:
	/* 내가 넣어준 경로에 해당하는 파일의 정보를 읽어서 aiScene객체에 저장해준다. */
	Assimp::Importer				m_Importer;

	/* 파일로부터 읽어온 모든 정보를 보관하고 있는다. */
	const aiScene*					m_pAIScene = { nullptr };
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private: 
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

	vector<MaterialTexturesPath>	m_MaterialTexturesPathes;

private:
	string						m_szModelName = {};
	
private:
	vector<class CBone*> m_Bones = {};

public:
	_bool IsAnimFinished() const { return m_bIsFinished; }

private:
	vector<class CAnimation*> m_Animations = {};

	_bool m_isLoop = { false };
	_bool m_bIsFinished = { false };
	_uint m_iCurrentAnimIndex;	
	_uint m_iNextAnimIndex;		// 정해지지 않은 것을 표현할땐 항상 max로...
	_uint m_iNumAnimations = { 0 };
	static double s_dAnimToAnimSpeed;
public:
	static _double GetAnimToAnimSpeed() { return s_dAnimToAnimSpeed; }	

public:
	HRESULT Ready_Bones(const aiNode* pNode, _int iParentBoneIndex);
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& readStream);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END