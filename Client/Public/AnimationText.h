#pragma once

#include "Client_Defines.h"

#include "TextObject.h"

BEGIN(Client)

class CAnimationText :
    public CTextObject
{
protected:
	CAnimationText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimationText(const CAnimationText& Prototype);
	virtual ~CAnimationText() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CFPS_Arm* m_pPlayerArm = nullptr;

	_uint m_iAnimationIndex = 0;
	_bool m_bControlAnim = false;

public:
	void Switch_ControlAnim() { m_bControlAnim = !m_bControlAnim; }
	void Set_AnimIndex(_uint iAnimIndex) { m_iAnimationIndex = iAnimIndex; }
	void Set_Arm(class CFPS_Arm* pArm) { m_pPlayerArm = pArm; }
	void AnimControl();
	_bool IsControl() const { return m_bControlAnim; }
public:
	static CAnimationText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
