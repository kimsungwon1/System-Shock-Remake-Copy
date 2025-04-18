#pragma once
#include "Base.h"

BEGIN(Engine)

class CPixel_Picking :
    public CBase
{
private:
    CPixel_Picking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPixel_Picking() = default;

public:
    HRESULT Initialize(HWND hWnd);
    _bool Picking(_float3* pPickedPos);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    ID3D11Texture2D* m_pPickDepthTexture = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;
    HWND                    m_hWnd = {};
    _uint               m_iViewportWidth{}, m_iViewportHeight{};

public:
    static CPixel_Picking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
    virtual void Free() override;
};

END