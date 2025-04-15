#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc, _int iIndex);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	_int	Get_Index() const { return m_iIndex; }
	void Set_LightDesc(const LIGHT_DESC& LightDesc);
private:
	LIGHT_DESC		m_LightDesc{};
	_int			m_iIndex{};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc, _int iIndex);
	virtual void Free() override;
};

END
