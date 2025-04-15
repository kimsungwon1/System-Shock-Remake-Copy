#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& Prototype);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype(_vector scale);
	virtual HRESULT Initialize(void* pArg) override;

	void UpdateRotation();

public:
	static CVIBuffer_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _vector scale);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	float rotationAngle = 0.0f;
	bool rotateLeft = true;
};

END