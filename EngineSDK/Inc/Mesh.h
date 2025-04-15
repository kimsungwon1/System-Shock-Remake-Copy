#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
	struct MeshFace {
		_uint iIndices[3] = {0,};
	};
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader* pShader, const char* pConstantName);

public:
	virtual _bool isPicking(const _float4x4& WorldMatrix, _float3* pOut) override;
	_bool LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance);
private:
	_bool lineIntersects_perTriangle(_fvector vPointA, _fvector vPointB, _fvector vPointC, _gvector vWorldRay_Pos, _hvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance);
public:
	HRESULT SaveMesh(const CModel* pModel, ofstream& saveStream, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT LoadMesh(const CModel* pModel, ifstream& loadStream);

private:
	_char				m_szName[MAX_PATH] = {};

	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };
	
	/*모델에 로드해놓은 전체 뼈 중의 인덱스를 이야기한다. */
	vector<_int>		m_BoneIndices;

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

	_uint				m_iNumTriangles{ 0 };
	MeshFace*			m_triangles = { nullptr };
	
	_float		m_fMaxX = {}; _float m_fMinX = {};
	_float		m_fMaxY = {}; _float m_fMinY = {};
	_float		m_fMaxZ = {}; _float m_fMinZ = {};

public:
	_float		GetMaxX() const { return m_fMaxX; } _float GetMinX() const { return m_fMinX; }
	_float		GetMaxY() const { return m_fMaxY; } _float GetMinY() const { return m_fMinY; }
	_float		GetMaxZ() const { return m_fMaxZ; } _float GetMinZ() const { return m_fMinZ; }

private:
	HRESULT	Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(const CModel* pModel, const aiMesh* pAIMesh);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, ifstream& loadStream);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END