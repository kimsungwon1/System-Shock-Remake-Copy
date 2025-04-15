#pragma once
#include "Camera.h"

BEGIN(Client)
class CFPSPlayer;

class CFPSCamera final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{

	}CAMERA_FPS_DESC;
private:
	CFPSCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFPSCamera(const CFPSCamera& Prototype);
	virtual ~CFPSCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fSensor = { 0.f };

public:
	void SetMatrix(const _float4x4* pMatrix);
	//CFPSPlayer* m_pPlayer = nullptr;
public:
	static CFPSCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
