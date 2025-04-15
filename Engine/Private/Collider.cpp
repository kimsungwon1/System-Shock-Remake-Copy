#include "Collider.h"
#include "GameInstance.h"

float roundToDecimalPlace(float value, int decimal_places) {
    float scale = std::powf(10.0f, decimal_places);
    return std::round(value * scale) / scale;
}

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& Prototype)
    : CComponent(Prototype)
    , m_eColliderType { Prototype.m_eColliderType }
#ifdef _DEBUG
    , m_pBatch { Prototype.m_pBatch }
    , m_pEffect { Prototype.m_pEffect}
    , m_pInputLayout { Prototype.m_pInputLayout }
#endif
{
#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
    m_eColliderType = eColliderType;

#ifdef _DEBUG

    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = nullptr;
    size_t      iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);
    
    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pArg);

    switch (m_eColliderType)
    {
    case TYPE_AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case TYPE_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case TYPE_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    }

    m_bIsPlayers = pBoundingDesc->bIsPlayers;
    m_pOwner = pBoundingDesc->pOwner;

    return S_OK;
}

void CCollider::Update(const _float4x4* pWorldMatrix)
{
    m_pBounding->Update(XMLoadFloat4x4(pWorldMatrix));
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pEffect->Apply(m_pContext);

    m_pBatch->Begin();

    if(FAILED(m_pBounding->Render(m_pBatch)))
        return E_FAIL;

    m_pBatch->End();


    return S_OK;
}
#endif

_bool CCollider::IsPicking(const _float4x4& WorldMatrix, _float3* pOut)
{
    return m_pBounding->IsPicking(WorldMatrix, pOut);
}

_bool CCollider::Intersect(CCollider* pTargetCollider, _float3* pIntersectMount)
{
    /*if (m_pBounding->Intersect(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding)) {
        if (pIntersectMount != nullptr) {
            _vector Distancevec = XMLoadFloat3(&pTargetCollider->m_pBounding->Get_Center()) - XMLoadFloat3(&m_pBounding->Get_Center());
            Distancevec.m128_f32[0] = roundToDecimalPlace(Distancevec.m128_f32[0], 3);
            Distancevec.m128_f32[1] = roundToDecimalPlace(Distancevec.m128_f32[1], 3);
            Distancevec.m128_f32[2] = roundToDecimalPlace(Distancevec.m128_f32[2], 3);
            _vector myExtents = XMLoadFloat3(&m_pBounding->Get_vExtents());
            _vector targetExtents = XMLoadFloat3(&pTargetCollider->m_pBounding->Get_vExtents());
            
            if(m_eColliderType == TYPE_AABB || m_eColliderType == TYPE_OBB){
                if(pTargetCollider->m_eColliderType == TYPE_AABB || pTargetCollider->m_eColliderType == TYPE_OBB)
                {
                    XMStoreFloat3(pIntersectMount, Distancevec);
                }
                else
                {
                    XMStoreFloat3(pIntersectMount, XMVector3Normalize(Distancevec) * XMVector3Length(myExtents + targetExtents / sqrtf(2) - Distancevec));
                }
            }
            else {
                if (pTargetCollider->m_eColliderType == TYPE_AABB || pTargetCollider->m_eColliderType == TYPE_OBB)
                {
                    XMStoreFloat3(pIntersectMount, XMVector3Normalize(Distancevec) * XMVector3Length(myExtents / sqrtf(2) + targetExtents - Distancevec));
                }
                else
                {
                    XMStoreFloat3(pIntersectMount, XMVector3Normalize(Distancevec) * XMVector3Length(myExtents / sqrtf(2) + targetExtents / sqrtf(2) - Distancevec));
                }
            }
        }
        return true;
    }
    return false;*/
    return m_pBounding->Intersect(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding);
}

_bool CCollider::Intersect(_fvector vPos, _fvector vDir, _float fDistance)
{
    return m_pBounding->Intersect(vPos, vDir, fDistance);
}

const _float3* CCollider::Get_Extents() const
{
    return &m_pBounding->Get_vExtents();
}

const _float3* CCollider::Get_Center() const
{
    return &m_pBounding->Get_Center();
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
    CCollider* pInstance = new CCollider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CShader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    __super::Free();

#ifdef _DEBUG
    if (false == m_isCloned)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }
    Safe_Release(m_pInputLayout);
#endif

    Safe_Release(m_pBounding);
}
