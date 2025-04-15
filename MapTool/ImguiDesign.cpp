#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "FreeCamera.h"
#include "ToolObject.h"
#include "MainApp.h"
#include "MouseHandle.h"

#include "Door.h"

#include "Transform.h"

#include "MyNavigation.h"

#include "MyCell_Full.h"

#include "LightObject.h"

#include "GameInstance.h"

#include "ImguiDesign.h"

#undef max

std::string wstring_to_utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

CImguiDesign::CImguiDesign(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CImguiDesign::~CImguiDesign()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    Safe_Release(m_pDevice); Safe_Release(m_pContext);
}

void CImguiDesign::Present()
{
    ImGui::SetNextWindowSize(ImVec2((_float)m_Size.first, (_float)m_Size.second));
    //ImGui::SetCursorPos(ImVec2(_float(g_iWinSizeX - m_Size.first), 0.f));
    if (ImGui::Begin("Tool", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) || ImGui::IsAnyItemHovered())
        {
            m_bMouseOnGui = true;
        }
        else {
            m_bMouseOnGui = false;
        }
        _bool cliter = ImGui::Button("Terrain");
        if (cliter) {
            m_eCurSection = AllObjectType::TYPE_TILE;
        }
        ImGui::SameLine();

        _bool cliDeco = ImGui::Button("Deco");
        if (cliDeco) {
            m_eCurSection = AllObjectType::TYPE_DECO;
        }
        ImGui::SameLine();

        _bool cliUnit = ImGui::Button("Unit");
        if (cliUnit) {
            m_eCurSection = AllObjectType::TYPE_UNIT;
        }
        ImGui::SameLine();
        
        _bool cliItem = ImGui::Button("Item");
        if (cliItem) {
            m_eCurSection = AllObjectType::TYPE_ITEM;
        }
        ImGui::SameLine();

        _bool cliInteractive = ImGui::Button("Interactive");
        if (cliInteractive) {
            m_eCurSection = AllObjectType::TYPE_Interactive;
        }

        _bool cliLight = ImGui::Button("Light");
        if (cliLight) {
            m_eCurSection = AllObjectType::TYPE_Light;
        }


        if (ImGui::Button("Navigation")) {
            m_bEditNavi = !m_bEditNavi;
        }

        if (m_bEditNavi) {
            Present_EditNavigation();
            ImGui::End();
            return;
        }

        ImGui::BeginChild("ScrollingRegion", ImVec2(float(m_Size.first - 10), 300.f), true, ImGuiWindowFlags_HorizontalScrollbar);
        if (m_eCurSection == AllObjectType::TYPE_Light) {
            _bool bNormalLight = ImGui::Button("Normal Light");
            _bool bRedLight = ImGui::Button("Red Light");

            if (bNormalLight || bRedLight) {
                m_pMainApp->m_pMouseHandle->PopObject();

                CLightObject::LIGHTOBJECT_DESC desc = {};
                desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
                desc.szModelName = {}; desc.bPlaced = false; desc.eType = m_eCurSection;
                desc.light_description.eType = LIGHT_DESC::TYPE_POINT;
                desc.light_description.vAmbient = { 0.f,0.f,0.f,0.f }; 
                desc.light_description.fRange = m_fRange; desc.light_description.vDirection = m_vDirection; desc.light_description.vPosition = { 0.f,0.f,0.f,1.f };

                if (bNormalLight) {
                    m_vDiffuse = { 1.f, 1.f, 1.f, 1.f };
                }
                if (bRedLight) {
                    m_vDiffuse = { 1.f,0.f,0.f,0.f };
                }
                desc.light_description.vDiffuse = m_vDiffuse;
                desc.light_description.vSpecular = m_vDiffuse;

                CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                    TEXT("Handle"), TEXT("Prototype_GameObject_Light"), &desc);

                auto* pLight = dynamic_cast<CLightObject*>(CGameInstance::Get_Instance()->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Handle")));

                m_pMainApp->m_pMouseHandle->PushObject(pLight);
            }

            ImGui::InputFloat("Light Range", (_float*)&m_fRange);
            ImGui::InputFloat3("Light Diffuse(Color)", (_float*)&m_vDiffuse);

            auto* pLight = dynamic_cast<CLightObject*>(m_pMainApp->m_pMouseHandle->m_pObjectOnMouse);
            
            if(pLight != nullptr)
            {
                LIGHT_DESC desc = {};
                desc.vDirection = m_vDirection; desc.fRange = m_fRange;
                desc.vDiffuse = m_vDiffuse;
                pLight->Set_Desc(desc);
            }
        }
        else {
            if (m_pMainApp->m_pMouseHandle->m_pObjectOnMouse != nullptr &&
                m_pMainApp->m_pMouseHandle->m_pObjectOnMouse->GetType() == TYPE_Light) {
                m_pMainApp->m_pMouseHandle->PopObject();
            }
            for (int i = 0; i < m_pMainApp->GetRefModelPrototype(m_eCurSection).size(); i++) {
                auto& modelAndName = m_pMainApp->GetRefModelPrototype(m_eCurSection)[i];
                auto name = wstring_to_utf8(modelAndName.szName);
                ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
                ImVec2 buttonSize = ImVec2(textSize.x + 5, textSize.y + 5); // 버튼 크기 조정 (패딩 포함)

                if (ImGui::Button(name.c_str(), buttonSize)) {
                    m_pMainApp->m_pMouseHandle->PopObject();

                    CToolObject::TOOLOBJECT_DESC desc;
                    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                    XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
                    desc.szModelName = modelAndName.szName;
                    desc.bPlaced = false;
                    desc.eType = m_eCurSection;

                    if (lstrcmp(desc.szModelName.c_str(), TEXT("Prototype_Component_Model_Card")) == 0) {
                        CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                            TEXT("Handle"), TEXT("Prototype_GameObject_Card"), &desc);
                    }
                    else
                    {
                        CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                            TEXT("Handle"), TEXT("Prototype_GameObject_Default"), &desc);
                    }

                    auto* obj = CGameInstance::Get_Instance()->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Handle"));

                    m_pMainApp->m_pMouseHandle->PushObject(dynamic_cast<CToolObject*>(obj));
                }
            }
            if (m_eCurSection == AllObjectType::TYPE_TILE) {
                if (ImGui::Button("Door")) {
                    CToolObject::TOOLOBJECT_DESC desc;
                    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                    XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
                    desc.szModelName = TEXT("Door");
                    desc.bPlaced = false;
                    desc.eType = m_eCurSection;

                    CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        TEXT("Handle"), TEXT("Prototype_GameObject_Door"), &desc);

                    auto* obj = CGameInstance::Get_Instance()->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Handle"));

                    m_pMainApp->m_pMouseHandle->PushObject(dynamic_cast<CToolObject*>(obj));
                }
            }
        }
        ImGui::EndChild();

        ImGui::InputInt("Data", &m_iData);

        ImGui::InputFloat("Distance from Window", &m_fDistance,
            0.1f, std::numeric_limits<float>::max());
        ImGui::SliderFloat("Scale", &m_fHandledScale, 0.1f, 10.f);
        ImGui::SameLine();
        if (ImGui::Button("Initialize")) {
            m_fHandledScale = 1.f;
        }
        Present_Transformbuttons();
        Present_Transform90Degree();
        Present_PositionTranslate();

        if (m_pMainApp->m_pMouseHandle->GetObjectOnMouse() != nullptr)
        {
            string newName = wstring_to_utf8(m_pMainApp->m_pMouseHandle->GetObjectOnMouse()->GetModelName());
            ImGui::Text("%s", newName.c_str());
        }
        if (m_eCurSection != AllObjectType::TYPE_Light)
            Present_Animations();

        if (ImGui::Button("Clone")) {
            if (auto* pLight = dynamic_cast<CLightObject*>(m_pMainApp->m_pMouseHandle->GetObjectOnMouse())) {
                CLightObject::LIGHTOBJECT_DESC desc = {};
                desc.fRotationPerSec = 0.f; desc.fSpeedPerSec = 10.f;
                desc.transMat = pLight->GetTransformCom()->GetWorldMat();
                desc.bPlaced = true; desc.eType = pLight->GetType();
                desc.szModelName = {};
                desc.light_description = pLight->Get_Desc();

                CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                    CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Light"), &desc);
            }
            else {
                auto* pObj = m_pMainApp->m_pMouseHandle->GetObjectOnMouse();
                CToolObject::TOOLOBJECT_DESC desc = {};
                desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
                desc.transMat = pObj->GetTransformCom()->GetWorldMat();
                desc.bPlaced = true; desc.eType = pObj->GetType();
                desc.szModelName = pObj->GetModelName();

                if (lstrcmp(pObj->GetModelName().c_str(), TEXT("Door")) == 0)
                {
                    CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Door"),
                        &desc);
                }
                else if (lstrcmp(pObj->GetModelName().c_str(), TEXT("Prototype_Component_Model_Card")) == 0)
                {
                    CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Card"),
                        &desc);
                }
                else {
                    CGameInstance::Get_Instance()->Add_CloneObject_ToLayer(LEVEL_STATIC,
                        CToolObject::TypeToLayername[desc.eType], TEXT("Prototype_GameObject_Default"),
                        &desc);
                }
            }
        }

        if (ImGui::Button("Save")) {
            m_pMainApp->Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            m_pMainApp->Load();
        }
        processMouseScroll();
        Present_PickedPixel();
    }ImGui::End();
}

void CImguiDesign::InitForGameObject()
{
    m_fDistance = 10.f;
    m_fHandledScale = m_pMainApp->m_pMouseHandle->m_pObjectOnMouse->GetTransformCom()->Get_Scaled().x;
    if (m_pMainApp->m_pMouseHandle->m_pObjectOnMouse->GetType() == TYPE_Light) {
        auto* light = dynamic_cast<CLightObject*>(m_pMainApp->m_pMouseHandle->m_pObjectOnMouse);
        m_vDiffuse = light->Get_Desc().vDiffuse;
        m_fRange = light->Get_Desc().fRange;
    }
    m_iData = m_pMainApp->m_pMouseHandle->m_pObjectOnMouse->Get_Data();
}

void CImguiDesign::Present_Transformbuttons()
{
    _float moved = 0.03f;
    if (m_pMainApp->m_pMouseHandle->GetObjectOnMouse() == nullptr)
        return;
    auto* pObjectOnMouse = m_pMainApp->m_pMouseHandle->GetObjectOnMouse();
    auto up = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_UP);
    auto right = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_RIGHT);
    auto look = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_LOOK);

    ImGui::Text("Transform");

    if (ImGui::Button("UP")) {
        pObjectOnMouse->GetTransformCom()->Turn(right, moved);
    }
    ImGui::SameLine();
    if (ImGui::Button("Down")) {
        pObjectOnMouse->GetTransformCom()->Turn(right, -moved);
    }
    ImGui::SameLine();
    if (ImGui::Button("Left")) {
        pObjectOnMouse->GetTransformCom()->Turn(up, -moved);
    }
    ImGui::SameLine();
    if (ImGui::Button("Right")) {
        pObjectOnMouse->GetTransformCom()->Turn(up, moved);
    }
    ImGui::SameLine();
    if (ImGui::Button("Side_Left")) {
        pObjectOnMouse->GetTransformCom()->Turn(look, moved);
    }
    ImGui::SameLine();
    if (ImGui::Button("Side_Right")) {
        pObjectOnMouse->GetTransformCom()->Turn(look, -moved);
    }
    ImGui::SameLine();

    if (ImGui::Button("Init")) {
        auto scaled = pObjectOnMouse->GetTransformCom()->Get_Scaled();
        _vector right = { scaled.x, 0.f, 0.f };
        _vector up = { 0.f, scaled.y, 0.f };
        _vector look = { 0.f, 0.f, scaled.z };
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_RIGHT, right);
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_UP, up);
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_LOOK, look);
        m_pMainApp->m_pMouseHandle->m_posOffset = { 0.f,0.f,0.f };
    }
}

void CImguiDesign::Present_Transform90Degree()
{
    auto* pObjectOnMouse = m_pMainApp->m_pMouseHandle->GetObjectOnMouse();
    if (pObjectOnMouse == nullptr)
        return;

    auto right = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_RIGHT);
    auto up = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_UP);
    auto look = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_LOOK);
    auto pos = pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
    if (ImGui::Button("90 UP")) {
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_LOOK, up);
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_UP, -look);
    }
    ImGui::SameLine();
    if (ImGui::Button("90 Down")) {
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_UP, look);
        pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_LOOK, -up);
    }
    if (ImGui::Button("90 Left")) {
        pObjectOnMouse->GetTransformCom()->LookAt(pos - right);
    }
    ImGui::SameLine();
    if (ImGui::Button("90 Right")) {
        pObjectOnMouse->GetTransformCom()->LookAt(pos + right);
    }
}

void CImguiDesign::Present_PositionTranslate()
{
    auto* pObjectOnMouse = m_pMainApp->m_pMouseHandle->GetObjectOnMouse();
    if (pObjectOnMouse == nullptr)
        return;

    _vector posOffset = m_pMainApp->m_pMouseHandle->GetPosOffset();

    if (ImGui::Button("+X")) {
        posOffset.m128_f32[0] += 0.01f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-X")) {
        posOffset.m128_f32[0] += -0.01f;
    }
    ImGui::SameLine();
    if (ImGui::Button("+Y")) {
        posOffset.m128_f32[1] += 0.01f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-Y")) {
        posOffset.m128_f32[1] += -0.01f;
    }
    ImGui::SameLine();
    if (ImGui::Button("+Z")) {
        posOffset.m128_f32[2] += 0.01f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-Z")) {
        posOffset.m128_f32[2] += -0.01f;
    }

    if (ImGui::Button("++X")) {
        posOffset.m128_f32[0] += 0.05f;
    }
    ImGui::SameLine();
    if (ImGui::Button("--X")) {
        posOffset.m128_f32[0] += -0.05f;
    }
    ImGui::SameLine();
    if (ImGui::Button("++Y")) {
        posOffset.m128_f32[1] += 0.05f;
    }
    ImGui::SameLine();
    if (ImGui::Button("--Y")) {
        posOffset.m128_f32[1] += -0.05f;
    }
    ImGui::SameLine();
    if (ImGui::Button("++Z")) {
        posOffset.m128_f32[2] += 0.05f;
    }
    ImGui::SameLine();
    if (ImGui::Button("--Z")) {
        posOffset.m128_f32[2] += -0.05f;
    }
    _matrix mat = XMLoadFloat4x4(&pObjectOnMouse->GetTransformCom()->GetWorldMat());
    _vector objectPos = mat.r[3];
    objectPos += posOffset;
    pObjectOnMouse->GetTransformCom()->Set_State(CTransform::STATE_POSITION, 
        pObjectOnMouse->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + (posOffset - m_pMainApp->m_pMouseHandle->GetPosOffset()));
    XMStoreFloat3(&m_pMainApp->m_pMouseHandle->m_posOffset, posOffset);
    //m_pMainApp->m_pMouseHandle->SetPosOffset(posOffset);
}

void CImguiDesign::Present_Animations()
{
    CToolObject* objOnMouse = m_pMainApp->m_pMouseHandle->GetObjectOnMouse();
    if (objOnMouse == nullptr) {
        return;
    }
    ImGui::BeginChild("Animation", ImVec2(float(m_Size.first - 10), 200.f), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    float horizonalSum = 0.0f;
    size_t iAnimationNum = objOnMouse->Get_NumAnimation();
    size_t iNumAnimOffset = 0;
    for (; iNumAnimOffset * 50 < iAnimationNum; iNumAnimOffset++) {
        auto animationNames = objOnMouse->Get_AnimationNames_by50(iNumAnimOffset);

        for (size_t i = 0; i < animationNames.size(); i++) {
            auto& animationName = to_string(50 * iNumAnimOffset + i + 1);
            animationName += ". ";
            animationName += animationNames[i];

            if (animationName == "") {
                animationName = to_string(i + 1);
            }

            ImVec2 textSize = ImGui::CalcTextSize(animationName.c_str());
            ImVec2 buttonSize = ImVec2(textSize.x + 5, textSize.y + 5); // 버튼 크기 조정 (패딩 포함)

            if (horizonalSum + buttonSize.x > m_Size.first - 10.f || horizonalSum == 0.f) {
                horizonalSum = 0.f;
            }
            else {
                ImGui::SameLine();
            }

            if (ImGui::Button(animationName.c_str(), buttonSize)) {
                objOnMouse->SetUp_Animation(i, true);
            }
            horizonalSum += buttonSize.x;
        }

    }
    ImGui::EndChild();
}

void CImguiDesign::Present_EditNavigation()
{
    if (ImGui::Button("Save")) {
        m_pMainApp->SaveNavi();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        m_pMainApp->LoadNavi();
    }

    if (ImGui::Button("Create Cell")) {
        // Creat Cell 버튼으로 생성했던거 초기화
        InitializeCreateCell();
        m_bCellAdding = true;
        m_bCellPicking = false;
        m_iCellVertexIndex = 1;
        m_pMainApp->m_pMouseHandle->m_pNavigation->ClearLines();
        InitializeSides();
    }

    if (ImGui::Button("Pick Cell")) {
        // Create Cell 버튼으로 생성했던거 초기화
        InitializeCreateCell();
        m_bCellAdding = false;
        m_bCellPicking = true;
        m_pMainApp->m_pMouseHandle->m_pNavigation->ClearLines();
        InitializeSides();
    }


    if (m_iCellVertexIndex > 0) {
        ImGui::Text("Cell Vertices : ");
        string f = {};
        for (size_t i = 0; i < m_iCellVertexIndex; i++) {
            if (i >= 3) {
                break;
            }
            ImGui::InputFloat("A : ", &m_CellMaking[i].x);
            ImGui::InputFloat("B : ", &m_CellMaking[i].y);
            ImGui::InputFloat("C : ", &m_CellMaking[i].z);
            f = to_string(m_CellMaking[i].x); f += "f";
            f += to_string(m_CellMaking[i].y); f += "f";
            f += to_string(m_CellMaking[i].z); f += "f / ";
            ImGui::Text(f.c_str());
        }
        if (ImGui::Button("Do Create")) {
            auto* navi = m_pMainApp->m_pMouseHandle->m_pNavigation;
            navi->ClearLines();
            navi->AddCellFull(m_CellMaking);
            InitializeCreateCell();
        }
    }
    else if (m_bCellPicking) {        
        auto* pPickedCell = m_pMainApp->m_pMouseHandle->m_pPickedCell;
        auto* pNavi = m_pMainApp->m_pMouseHandle->m_pNavigation;
        m_bMakeCellFlag = false;
        if (pPickedCell == nullptr)
            return;

        auto va = pPickedCell->Get_Point(CMyCell_Full::POINT_A);
        auto vb = pPickedCell->Get_Point(CMyCell_Full::POINT_B);
        auto vc = pPickedCell->Get_Point(CMyCell_Full::POINT_C);

        _float3 vA = {};
        _float3 vB = {};
        _float3 vC = {};
        XMStoreFloat3(&vA, va); XMStoreFloat3(&vB, vb); XMStoreFloat3(&vC, vc);
        
        if (ImGui::Button("A-B")) {
            if (m_iSelectedSide != 1)
            {
                m_iSelectedSide = 1;
            }
            else
            {
                m_iSelectedSide = -1;
                m_pMainApp->m_pMouseHandle->m_pNavigation->ClearLines();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("B-C")) {
            if (m_iSelectedSide != 2)
            {
                m_iSelectedSide = 2;
            }
            else
            {
                m_iSelectedSide = -1;
                m_pMainApp->m_pMouseHandle->m_pNavigation->ClearLines();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("C-A")) {
            if (m_iSelectedSide != 3)
            {
                m_iSelectedSide = 3;
            }
            else
            {
                m_iSelectedSide = -1;
                m_pMainApp->m_pMouseHandle->m_pNavigation->ClearLines();
            }
        }

        if (ImGui::Button("Delete")) {
            m_pMainApp->m_pMouseHandle->m_pNavigation->DeleteCell(m_pMainApp->m_pMouseHandle->m_pPickedCell);
            m_pMainApp->m_pMouseHandle->m_pPickedCell = nullptr;
            m_iSelectedSide = -1;
        }
        ImGui::InputFloat("x", &m_fAddCellVertex.x, 0.f, 0.f, "%.6f");
        ImGui::InputFloat("y", &m_fAddCellVertex.y, 0.f, 0.f, "%.6f");
        ImGui::InputFloat("z", &m_fAddCellVertex.z, 0.f, 0.f, "%.6f");
        if (ImGui::Button("Do Create")) {
            if (m_iSelectedSide == 1)
                vC = m_fAddCellVertex;
            else if (m_iSelectedSide == 2)
                vA = m_fAddCellVertex;
            else if (m_iSelectedSide == 3)
                vB = m_fAddCellVertex;
            _float3 Cell[3] = {};
            Cell[0] = vA; Cell[1] = vB; Cell[2] = vC;
            pNavi->AddCellFull(Cell);
            pNavi->ClearLines();
            InitializeSides();
        }
        string f = {};
        f += "A : "; f += to_string(vA.x); f += "f "; f += to_string(vA.y); f += "f "; f += to_string(vA.z); f += '\n';
        f += "B : "; f += to_string(vB.x); f += "f "; f += to_string(vB.y); f += "f "; f += to_string(vB.z); f += '\n';
        f += "C : "; f += to_string(vC.x); f += "f "; f += to_string(vC.y); f += "f "; f += to_string(vC.z);;
        ImGui::Text(f.c_str());        
    }
}

void CImguiDesign::Present_PickedPixel()
{
    auto* pGame = CGameInstance::Get_Instance();

    pGame->Pixel_Picking(&m_vPixelPos);

    string szPos = to_string(m_vPixelPos.x) + string("f ") + to_string(m_vPixelPos.y) + string("f ") + to_string(m_vPixelPos.z) + string("f");
    ImGui::Text(szPos.c_str());
}

void CImguiDesign::InitializeCreateCell()
{
    m_iCellVertexIndex = 0;
    for (int i = 0; i < 3; i++) {
        m_CellMaking[i] = {};
    }
}

void CImguiDesign::AddVertextToMakingCell()
{
    if (m_iCellVertexIndex < 3) {
        m_iCellVertexIndex++;
    }
    else {
        m_iCellVertexIndex = 0;
    }
}


void CImguiDesign::processMouseScroll()
{
    ImGuiIO& io = ImGui::GetIO();

    // 마우스 휠 스크롤 값 얻기
    float mouseWheel = io.MouseWheel;

    // 마우스 휠 스크롤 값 출력
    if (mouseWheel != 0.0f && !m_bMouseOnGui)
    {
        m_fDistance += mouseWheel;
    }
}
