#include "MyCell_Line.h"

CMyCell_Line::CMyCell_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMyCell_Part(pDevice, pContext)
{
}

HRESULT CMyCell_Line::Initialize(const _float3* pPoints, _int iIndex)
{
    m_vPoints[0] = pPoints[0];
    m_vPoints[1] = pPoints[1];

    m_pStates = new DirectX::CommonStates(m_pDevice);

    m_pEffect = new DirectX::BasicEffect(m_pDevice);
    m_pEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_pEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount, shaderByteCode,
        byteCodeLength, m_inputLayout.ReleaseAndGetAddressOf())))
        return E_FAIL;

    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

    return S_OK;
}

HRESULT CMyCell_Line::Initialize_Line(_matrix naviMat, _vector vColor)
{
    XMStoreFloat4x4(&m_NavigationWorldMatrix, naviMat);
    XMStoreFloat4(&m_vColor, vColor);

    return S_OK;
}

HRESULT CMyCell_Line::Render()
{
    _vector v1 = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat4x4(&m_NavigationWorldMatrix));
    _vector v2 = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[1]), XMLoadFloat4x4(&m_NavigationWorldMatrix));

    m_pContext->OMSetBlendState(m_pStates->Opaque(), nullptr, 0xffffffff);
    m_pContext->OMSetDepthStencilState(m_pStates->DepthNone(), 0);
    m_pContext->RSSetState(m_pStates->CullNone());

    m_pEffect->Apply(m_pContext);

    m_pContext->IASetInputLayout(m_inputLayout.Get());

    m_pBatch->Begin();

    VertexPositionColor vertex1(v1, Colors::Red/*XMLoadFloat4(&m_vColor)*/);
    VertexPositionColor vertex2(v2, Colors::Red/*XMLoadFloat4(&m_vColor)*/);
    m_pBatch->DrawLine(vertex1, vertex2);

    m_pBatch->End();

    return S_OK;
}

CMyCell_Line* CMyCell_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
    const _float3* pPoints, _int iIndex, _matrix naviMat, _vector vColor)
{
    CMyCell_Line* pInstance = new CMyCell_Line(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex))) {
        MSG_BOX(TEXT("Failed to Create : CLine"));
        Safe_Release(pInstance);
    }
    if (FAILED(pInstance->Initialize_Line(naviMat, vColor))) {
        MSG_BOX(TEXT("Failed to Create : CLine - line Init"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyCell_Line::Free()
{
    __super::Free();
    Safe_Delete(m_pStates);
    Safe_Delete(m_pEffect);
    Safe_Delete(m_pBatch);
    Safe_Release(m_inputLayout);
}
