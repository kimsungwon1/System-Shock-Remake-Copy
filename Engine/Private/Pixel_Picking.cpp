#include "Pixel_Picking.h"

#include "GameInstance.h"

CPixel_Picking::CPixel_Picking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPixel_Picking::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;
	_uint		iNumViewports = { 1 };

	D3D11_VIEWPORT ViewportDesc{};
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_TEXTURE2D_DESC TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_iViewportWidth = ViewportDesc.Width;
	TextureDesc.Height = m_iViewportHeight = ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;
	/* 어떤 용도로 바인딩 될수 있는 View타입의 텍스처를 만들기 위한 Texture2D입니까? */
	TextureDesc.BindFlags = 0;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickDepthTexture)))
		return E_FAIL;
	return S_OK;
}

_bool CPixel_Picking::Picking(_float3* pPickedPos)
{
	if (FAILED(m_pGameInstance->Copy_RenderTarget(TEXT("Target_PickDepth"), m_pPickDepthTexture)))
		return false;

	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	if (ptMouse.x < 0) {
		ptMouse.x = 0;
	}
	if (ptMouse.y < 0) {
		ptMouse.y = 0;
	}
	if (ptMouse.x >= m_iViewportWidth) {
		ptMouse.x = m_iViewportWidth - 1;
	}
	if (ptMouse.y >= m_iViewportHeight) {
		ptMouse.y = m_iViewportHeight - 1;
	}

	_uint iPixelIndex = ptMouse.y * m_iViewportWidth + ptMouse.x;

	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pPickDepthTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	_float	fProjZ = ((_float4*)SubResource.pData)[iPixelIndex].x;
	_bool isPicked = static_cast<_bool>(((_float4*)SubResource.pData)[iPixelIndex].w);

	m_pContext->Unmap(m_pPickDepthTexture, 0);

	if (!isPicked)
		return false;

	_float4 vProjPos;

	/* 투영공간상의 위치 */
	vProjPos.x = ptMouse.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = ptMouse.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fProjZ;
	vProjPos.w = 1.f;

	/* 뷰 공간상의 위치*/
	_vector vViewPos = XMVector3TransformCoord(XMLoadFloat4(&vProjPos), m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ));

	_vector vWorldPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW));

	XMStoreFloat3(pPickedPos, vWorldPos);

	return _bool();
}

CPixel_Picking* CPixel_Picking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	CPixel_Picking* pInstance = new CPixel_Picking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CPixel_Picking"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPixel_Picking::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pPickDepthTexture);
}
