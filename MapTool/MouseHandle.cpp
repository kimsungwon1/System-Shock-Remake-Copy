#include "MouseHandle.h"
#include "GameInstance.h"

#include "Transform.h"
#include "LightObject.h"
#include "FreeCamera.h"

#include "MyNavigation.h"
#include "MyCell_Full.h"

#include "Terrain.h"

#include "Door.h"

#include "ImguiDesign.h"
#include "Client_Defines.h"

float roundToDecimalPlace(float value, int decimal_places) {
    float scale = std::powf(10.0f, (float)decimal_places);
    return std::floor(value * scale) / scale;
}

CMouseHandle::CMouseHandle(CImguiDesign* pGui)
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    m_pGui = pGui;

    Safe_AddRef(m_pGameInstance);
}

HRESULT CMouseHandle::PushObject(CToolObject* pObj)
{
    if (pObj == nullptr) {
        return E_FAIL;
    }
    if (m_pObjectOnMouse != nullptr) {
        if (FAILED(PopObject())) {
            return E_FAIL;
        }
    }
    m_pObjectOnMouse = pObj;

    return S_OK;
}

HRESULT CMouseHandle::PopObject()
{
    if (m_pObjectOnMouse == nullptr) {
        return E_FAIL;
    }

    if (!m_pObjectOnMouse->IsPlaced()) {
        m_pObjectOnMouse = nullptr;
        return S_OK;
    }

    HRESULT hr = m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, L"Handle", m_pObjectOnMouse);

    if (FAILED(hr)) {
        return E_FAIL;
    }
    m_pObjectOnMouse = nullptr;

    return S_OK;
}

HRESULT CMouseHandle::CloneObject()
{
    if (m_pObjectOnMouse == nullptr) {
        return E_FAIL;
    }
    return m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, 
        m_pObjectOnMouse->GetLayerTag(), m_pObjectOnMouse->GetPrototypeName());
}

void CMouseHandle::Update(_float fTimeDelta)
{
    if (m_pNavigation == nullptr)
        m_pNavigation = dynamic_cast<CMyNavigation*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Layer_Navigation")));

    // 들고있는거 삭제
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_ESCAPE) || m_pGui->IsCellPicking()) {
        m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, TEXT("Handle"), m_pObjectOnMouse);
        m_pObjectOnMouse = nullptr;
        //return;
    }

    // 지형- 타일에 브러쉬
    Brush();

    if (m_pGui->IsCellEditing()) {
        Edit_Navigation();
    }
    
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_X)) {
        m_bCross = !m_bCross;
    }
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_C)) {
        m_bPickToTile = !m_bPickToTile;
    }

    // Tile, Unit Picking
    if (m_pObjectOnMouse == nullptr && m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !m_pGui->IsLMouseOnGui()) {
        _float fMinDistance = D3D11_FLOAT32_MAX;
        _float3 pickedPos = {};
        CGameObject* pPickedObj = nullptr;
        if (m_pGui->GetSectionType() == TYPE_Light) {
            Picking_Object(pPickedObj, &pickedPos, TEXT("Light"), fMinDistance);
        }
        else
        {
            Picking_Object(pPickedObj, &pickedPos, TEXT("Tile"), fMinDistance);
            Picking_Object(pPickedObj, &pickedPos, TEXT("Unit"), fMinDistance);
            Picking_Object(pPickedObj, &pickedPos, TEXT("Deco"), fMinDistance);
            Picking_Object(pPickedObj, &pickedPos, TEXT("Item"), fMinDistance);
            Picking_Object(pPickedObj, &pickedPos, TEXT("Interactive"), fMinDistance);
        }
        m_pObjectOnMouse = dynamic_cast<CToolObject*>(pPickedObj);
        if (m_pObjectOnMouse != nullptr) {
            m_pGui->InitForGameObject();
        }
    }

    Update_HandleNotPlacedObject();
    Update_HandlePlacedObject();  //주로 이미 배치된 얘를 컨트롤.

    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_DELETE)) {
        m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, m_pObjectOnMouse->GetLayerTag(), m_pObjectOnMouse);
        m_pObjectOnMouse = nullptr;
    }
}

_bool CMouseHandle::Picking_Object(CGameObject*& pRetObject, _float3* pos, _wstring layerName, _float& fMinDistance)
{
    _vector cameraPos = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[3];

    _float3 pickedPos = {};
    auto* pObjects = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, layerName);
    if (pObjects == nullptr) {
        return false;
    }

    for (auto* pObj : *pObjects) {
        if (pObj->IsPicking(&pickedPos)) {
            _float dist = XMVector3Length(cameraPos - XMLoadFloat3(&pickedPos)).m128_f32[0];
            dist = roundToDecimalPlace(dist, 1);
            if (dist < fMinDistance) {
                pRetObject = pObj;
                fMinDistance = dist;
                pickedPos.x = roundToDecimalPlace(pickedPos.x, 2);
                pickedPos.y = roundToDecimalPlace(pickedPos.y, 2);
                pickedPos.z = roundToDecimalPlace(pickedPos.z, 2);
                *pos = pickedPos;
            }
        }
    }

    if (pRetObject != nullptr) {
        return true;
    }
    else
        return false;
}

_vector CMouseHandle::GetCrossPosition(_vector vPosition)
{
    _float fOffset = m_pObjectOnMouse->GetLength();
    
    _int iOffsetX = {}; _int iOffsetY = {}; _int iOffsetZ = {};
    iOffsetX = vPosition.m128_f32[0] / fOffset;
    iOffsetY = vPosition.m128_f32[1] / fOffset;
    iOffsetZ = vPosition.m128_f32[2] / fOffset;
    
    _vector retVec = {iOffsetX * fOffset, iOffsetY * fOffset, iOffsetZ * fOffset };

    if (m_bPickToTile) {
        retVec.m128_f32[1] = vPosition.m128_f32[1];
    }

    return retVec;
}

HRESULT CMouseHandle::SaveNavi(ofstream& ofs)
{
    m_pNavigation->SaveNavi(ofs);

    return S_OK;
}

HRESULT CMouseHandle::LoadNavi(ifstream& ifs)
{
    m_pNavigation->LoadNavi(ifs);

    return S_OK;
}

HRESULT CMouseHandle::ClearNavi()
{
    return m_pNavigation->ClearAll();
}

void CMouseHandle::Update_HandleNotPlacedObject()
{
    if (m_pObjectOnMouse == nullptr || m_pObjectOnMouse->IsPlaced())
        return;
    // 맞추기
    if (!CFreeCamera::IsFixed() && !m_pGui->IsLMouseOnGui()) {
        // 기본지형 - 지형오브젝트를 피킹하여 Place.
        _float fMinDistance = D3D11_FLOAT32_MAX;
        _float3 pickedPos = {};
        CGameObject* pPickedObj = nullptr;
        Picking_Object(pPickedObj, &pickedPos, TEXT("Terrain"), fMinDistance);
        pickedPos.y = pickedPos.y + 0.1f;
        Picking_Object(pPickedObj, &pickedPos, TEXT("Tile"), fMinDistance);

        //카메라 위치 뽑기
        _vector cameraPos = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[3];
        auto CameraWorldMat = m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
        auto CameraLook = XMVector3Normalize(XMLoadFloat4((_float4*)&CameraWorldMat.m[CTransform::STATE_LOOK]));
        auto frontOfCamera = cameraPos + CameraLook * m_pGui->GetDistance() + XMLoadFloat3(&m_posOffset);

        // 마우스 오브젝트 스케일 뽑기
        auto Scale = m_pObjectOnMouse->GetTransformCom()->Get_Scaled();

        _vector position = {};

        // 마우스 오브젝트 위치 세팅 - 피킹한 오브젝트 없으면 걍 카메라 앞에
        if (!m_bPickToTile)
            position = frontOfCamera;
        // 피킹 오브젝트 있으면 피킹한 놈 위에
        else {
            position = XMLoadFloat3(&pickedPos);
        }

        // 격자 모드면 격자 세팅
        if (m_bCross) {
            position = GetCrossPosition(position);
        }
        position += XMLoadFloat3(&m_posOffset);

        m_pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_POSITION,
            position);

        // GUI의 스케일 세팅 가져오기- 그걸 마우스 오브젝트에 세팅
        _float fs = m_pGui->GetHandledScale();
        m_pObjectOnMouse->GetTransformCom()->Set_Scaled(fs, fs, fs);

        // 마우스 클릭- 마우스 오브젝트와 똑같은 오브젝트를 피킹한 위치에 걸기
        
        //desc.
        //클릭시 행동
        if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
            if (m_pObjectOnMouse->GetType() == TYPE_Light) {
                CLightObject::LIGHTOBJECT_DESC desc = {};
                desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                desc.transMat = m_pObjectOnMouse->GetTransformCom()->GetWorldMat();
                desc.bPlaced = true; desc.eType = m_pObjectOnMouse->GetType();
                desc.szModelName = m_pObjectOnMouse->GetModelName();
                desc.light_description = dynamic_cast<CLightObject*>(m_pObjectOnMouse)->Get_Desc();
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
                    CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Light"),
                    &desc);
            }
            else {
                CDoor::DATAOBJ_DESC desc = {};
                desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                desc.transMat = m_pObjectOnMouse->GetTransformCom()->GetWorldMat();
                desc.bPlaced = true; desc.eType = m_pObjectOnMouse->GetType();
                desc.szModelName = m_pObjectOnMouse->GetModelName();
                desc.iData = m_pGui->GetObjData();
                if (lstrcmp(desc.szModelName.c_str(), TEXT("Door")) == 0)
                {
                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Door"),
                        &desc);
                }
                else if (lstrcmp(desc.szModelName.c_str(), TEXT("Prototype_Component_Model_Card")) == 0)
                {
                    CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Card"),
                        &desc);
                }
                else
                {
                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Default"),
                        &desc);
                }
            }
        }

    }
    else {
        GetCursorPos(&originalCursorPoint);
        //m_pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_POSITION, frontOfCamera);
    }
}

void CMouseHandle::Update_HandlePlacedObject()
{
    if (m_pObjectOnMouse == nullptr || !m_pObjectOnMouse->IsPlaced())
        return;
    _float fMinDistance = D3D11_FLOAT32_MAX;
    CGameObject* pPickedObj = nullptr;
    _float3 pickedPos = {};

    auto originalWorldMat = m_pObjectOnMouse->GetTransformCom()->GetWorldMat();

    _float fs = m_pGui->GetHandledScale();
    m_pObjectOnMouse->GetTransformCom()->Set_Scaled(fs, fs, fs);



    //m_pGui->SetObjData(m_pObjectOnMouse->Get_Data());
    m_pObjectOnMouse->Set_Data(m_pGui->GetObjData());
}

void CMouseHandle::Edit_Navigation()
{
    if(m_pGui->IsCellAdding())
    {
        if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !m_pGui->IsLMouseOnGui()) {

            if (m_pGui->GetCellVertexIndex() > 0) {
                _float fMinDistance = D3D11_FLOAT32_MAX;
                _float3 pickedPos = {};
                CGameObject* pPickedObj = nullptr;
                Picking_Object(pPickedObj, &pickedPos, TEXT("Terrain"), fMinDistance);
                pickedPos.y = pickedPos.y + 0.1f;
                Picking_Object(pPickedObj, &pickedPos, TEXT("Tile"), fMinDistance);

                pickedPos = m_pNavigation->Get_AlmostSamePoint(pickedPos);

                if (m_pGui->GetCellVertexIndex() == 1) {
                    // 점 픽해서 만들기
                    _float3 vPoints[3];
                    m_pGui->SetMakingCell(0, pickedPos);
                    vPoints[0] = m_pGui->GetMakingCell(0);
                    vPoints[1] = m_pGui->GetMakingCell(0);
                    vPoints[2] = m_pGui->GetMakingCell(0);

                    m_pGui->AddVertextToMakingCell();
                }
                else if (m_pGui->GetCellVertexIndex() == 2) {
                    // 직선 픽해서 만들기
                    m_pGui->SetMakingCell(1, pickedPos);
                    m_pGui->AddVertextToMakingCell();

                    _float3 vPoints[3];
                    vPoints[0] = m_pGui->GetMakingCell(0);
                    vPoints[1] = m_pGui->GetMakingCell(1);
                    vPoints[2] = m_pGui->GetMakingCell(1);

                    m_pNavigation->AddCellLine(vPoints);
                }
                else if (m_pGui->GetCellVertexIndex() == 3) {
                    // 삼각형 최종 픽해서 만들기.
                    m_pGui->SetMakingCell(2, pickedPos);
                    m_pGui->AddVertextToMakingCell();

                    _float3 vPoints[3];
                    vPoints[0] = m_pGui->GetMakingCell(0);
                    vPoints[1] = m_pGui->GetMakingCell(1);
                    vPoints[2] = m_pGui->GetMakingCell(2);

                    m_pNavigation->ClearLines();
                    m_pNavigation->AddCellFull(vPoints);
                    m_pGui->InitializeCreateCell();
                }
            }
        }
    }
    // 셀 만들기
    
    // 셀 피킹
    else if (m_pGui->IsCellPicking()) {
        m_pGui->InitializeCreateCell();
        _float3 vIn = {};
        _int iSide = m_pGui->GetSelectedSide();
        if (iSide > -1) {
            _float3 vPoints[3] = {};
            
            auto vA = m_pPickedCell->Get_Point(CMyCell_Full::POINT_A);
            auto vB = m_pPickedCell->Get_Point(CMyCell_Full::POINT_B);
            auto vC = m_pPickedCell->Get_Point(CMyCell_Full::POINT_C);

            // AB - 1 BC - 2 CA - 3
            if (iSide == 1) {
                XMStoreFloat3(&vPoints[0], vA);
                XMStoreFloat3(&vPoints[1], vB);
                XMStoreFloat3(&vPoints[2], vB);
            }
            else if (iSide == 2) {
                XMStoreFloat3(&vPoints[0], vB);
                XMStoreFloat3(&vPoints[1], vC);
                XMStoreFloat3(&vPoints[2], vC);
            }
            else if (iSide == 3) {
                XMStoreFloat3(&vPoints[0], vC);
                XMStoreFloat3(&vPoints[1], vA);
                XMStoreFloat3(&vPoints[2], vA);
            }

            if (iSide >= 1 && iSide <= 3) {
                for (int i = 0; i < 3; i++) {
                    vPoints[i].y = vPoints[i].y + 0.1f;
                    m_pNavigation->ClearLines();
                    m_pNavigation->AddCellLine(vPoints);
                }
            }

            if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !m_pGui->IsLMouseOnGui()) {
                _float fMinDistance = D3D11_FLOAT32_MAX;
                _float3 pickedPos = {};
                CGameObject* pPickedObj = nullptr;
                Picking_Object(pPickedObj, &pickedPos, TEXT("Terrain"), fMinDistance);
                pickedPos.y = pickedPos.y + 0.1f;
                Picking_Object(pPickedObj, &pickedPos, TEXT("Tile"), fMinDistance);

                pickedPos = m_pNavigation->Get_AlmostSamePoint(pickedPos);

                if (iSide == 1) {
                    vPoints[0].y = vA.m128_f32[1];
                    vPoints[1].y = vB.m128_f32[1];
                }
                if (iSide == 2) {
                    vPoints[0].y = vB.m128_f32[1];
                    vPoints[1].y = vC.m128_f32[1];
                }
                if (iSide == 3) {
                    vPoints[0].y = vC.m128_f32[1];
                    vPoints[1].y = vA.m128_f32[1];
                }
                /*for (int i = 0; i < 3; i++) {
                    vPoints[i].y = vA.m128_f32[1];
                }*/

                vPoints[2] = pickedPos;

                m_pNavigation->AddCellFull(vPoints);
                m_pNavigation->ClearLines();
                m_pGui->InitializeSides();
            }
        }

        else if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !m_pGui->IsLMouseOnGui()) {
            _vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();
            auto* pickedCell = m_pNavigation->Cell_Picking(vCamPosition, &vIn);
            if (pickedCell != nullptr) {
                m_pPickedCell = pickedCell;
            }

        }


    }
}

void CMouseHandle::Brush()
{
    _float fMinDistance = D3D11_FLOAT32_MAX;
    _float3 pickedPos = {};
    CGameObject* pPickedObj = nullptr;
    Picking_Object(pPickedObj, &pickedPos, TEXT("Terrain"), fMinDistance);
    pickedPos.y = pickedPos.y + 0.1f;
    Picking_Object(pPickedObj, &pickedPos, TEXT("Tile"), fMinDistance);


    if (auto* pTerrain = dynamic_cast<CTerrain*>(pPickedObj)) {
        _float4 pickedVec = {}; 
        XMStoreFloat4(&pickedVec, XMVectorSet(pickedPos.x, pickedPos.y, pickedPos.z, 1.f));
        pTerrain->Bind_BrushPos(&pickedVec);
    }
    if (auto* pToolObj = dynamic_cast<CToolObject*>(pPickedObj)) {
        _float4 pickedVec = {};
        XMStoreFloat4(&pickedVec, XMVectorSet(pickedPos.x, pickedPos.y, pickedPos.z, 1.f));
        pToolObj->Bind_BrushPos(&pickedVec, true);
    }
}

CMouseHandle* CMouseHandle::Create(CImguiDesign* pGui)
{
    auto* pInstance = new CMouseHandle(pGui);

    return pInstance;
}

void CMouseHandle::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pObjectOnMouse);
}
