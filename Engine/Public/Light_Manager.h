#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight_Manager : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc, _int iIndex);
	HRESULT Change_Light(const LIGHT_DESC& LightDesc, _int iIndex);
	HRESULT Delete_Light(_int iIndex);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLight*>		m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};
END
