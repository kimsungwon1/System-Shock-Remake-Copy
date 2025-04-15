#include "VIBuffer_Mesh_Instance.h"

#include "Model.h"
#include "Bone.h"
#include "Animation.h"
#include "Mesh.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
    : CVIBuffer_Instancing { Prototype }
{
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, ifstream& loadStream)
{
    if (FAILED(__super::Initialize_Prototype(Desc)))
        return E_FAIL;

	_uint		iFlag = { 0 };

	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	_uint iType = {};
	_float4x4 preTransformMatrix = {};
	auto size = sizeof(preTransformMatrix);
	loadStream.read(reinterpret_cast<char*>(&preTransformMatrix), sizeof(preTransformMatrix));
	loadStream.read(reinterpret_cast<char*>(&iType), sizeof(_uint));
	if (iType == CModel::TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	// Bones read
	_uint iNumBones = {};
	loadStream.read(reinterpret_cast<char*>(&iNumBones), sizeof(_uint));

	// Meshes read
	//{
	//	_int iNumMeshes = {};
	//	loadStream.read(reinterpret_cast<char*>(&iNumMeshes), sizeof(iNumMeshes));
	//	for (size_t i = 0; i < iNumMeshes; i++)
	//	{
	//		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, loadStream);
	//		if (nullptr == pMesh)
	//			return E_FAIL;

	//		m_Meshes.emplace_back(pMesh);
	//	}
	//}
	//// Material read
	//{
	//	loadStream.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(m_iNumMaterials));
	//	_uint mtpSize = {};
	//	loadStream.read(reinterpret_cast<char*>(&mtpSize), sizeof(mtpSize));
	//	for (int i = 0; i < mtpSize; i++) {
	//		_uint	materialIndex = {};
	//		_uint	textureType = {};
	//		_tchar	pathName[MAX_PATH] = {};
	//		_uint pathSize = 0;
	//		loadStream.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));
	//		loadStream.read(reinterpret_cast<char*>(&textureType), sizeof(textureType));
	//		loadStream.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
	//		loadStream.read(reinterpret_cast<char*>(pathName), sizeof(_tchar) * pathSize);
	//		_wstring finalPath = pathName;
	//		m_MaterialTexturesPathes.emplace_back(
	//			MaterialTexturesPath{ materialIndex, textureType, finalPath }
	//		);
	//	}
	//	for (size_t i = 0; i < m_iNumMaterials; i++) {
	//		_bool noItis = true;
	//		MESH_MATERIAL MeshMaterial{};

	//		for (size_t j = 0; j < m_MaterialTexturesPathes.size(); j++) {
	//			if (i == m_MaterialTexturesPathes[j].materialIndex) {
	//				MeshMaterial.pMaterialTextures[m_MaterialTexturesPathes[j].textureType]
	//					= CTexture::Create(m_pDevice, m_pContext, m_MaterialTexturesPathes[j].path.c_str(), 1);
	//			}
	//		}
	//		m_Materials.emplace_back(MeshMaterial);
	//	}
	//}
	//// Animation read
	//{
	//	loadStream.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

	//	for (size_t i = 0; i < m_iNumAnimations; i++)
	//	{
	//		CAnimation* pAnim = CAnimation::Create(loadStream);
	//		m_Animations.emplace_back(pAnim);
	//	}
	//}

	////ETC Read
	//loadStream.read(reinterpret_cast<char*>(&m_iNextAnimIndex), sizeof(_uint));

    return E_NOTIMPL;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

void CVIBuffer_Mesh_Instance::Update(_float fTimeDelta)
{
}

CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
    return nullptr;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
    return nullptr;
}

void CVIBuffer_Mesh_Instance::Free()
{
}
