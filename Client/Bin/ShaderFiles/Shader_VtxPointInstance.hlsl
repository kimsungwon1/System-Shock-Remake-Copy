#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : COLOR;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
};

struct VS_UI_OUT
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : COLOR;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    
    /*���û��¿����� ������ ���� */
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    /*���忡 Ư�� ���·� ��ġ�Ѵ�. */
    vPosition = mul(vPosition, g_WorldMatrix);
    
    Out.vPosition = vPosition;
    Out.vPSize = In.vPSize;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

VS_OUT VS_POSWORLD(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /*���û��¿����� ������ ���� */
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    //vPosition = mul(vPosition, g_WorldMatrix);
    
    Out.vPosition = vPosition;
    Out.vPSize = In.vPSize;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

VS_UI_OUT VS_UI(VS_IN In)
{
    VS_UI_OUT Out = (VS_UI_OUT) 0;
    
    /*���û��¿����� ������ ���� */
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    //vPosition = mul(vPosition, g_WorldMatrix);
    
    Out.vPosition = vPosition;
    Out.vPSize = In.vPSize;
    Out.vLifeTime = In.vLifeTime;
    Out.TransformMatrix = In.TransformMatrix;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
};

struct GS_UI_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : COLOR;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    
    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip();
    
    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)]
void GS_BLUE(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    
    float3 vLook = float3(0.f, 0.f, 1.f);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    
    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip();
    
    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

[maxvertexcount(6)]
void GS_UI(point GS_UI_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    
    float3 vLook = (0.f, 0.f, 1.f); //(g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = In[0].TransformMatrix._11_12_13; //normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = In[0].TransformMatrix._21_22_23; //normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    
    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip();
    
    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}
/* Triangle :  ���� ������ �� VS_main�� ����Ҷ����� ���*/
/* ������ ��� ����Ǹ�, ���� ������ ���� */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ����*/
/* ������ ��ġ�� ����Ʈ�� ��ȯ */
/* �����Ͷ����� : �������� ������� �ȼ� ������� */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if(Out.vColor.a <= 0.3f)
        discard;
    
    Out.vColor.rgb = (float3) 0.f;
    Out.vColor.r = In.vLifeTime.y / In.vLifeTime.x;
    Out.vColor.a = Out.vColor.a * (1.f - (In.vLifeTime.y / In.vLifeTime.x));
    
    if(In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    
    return Out;
}

PS_OUT PS_Blue(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor.rgba = vector(0.f, 0.5f, 1.f, 1.f);
    
    return Out;
}

PS_OUT PS_UI_INVENSLOT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Paticle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    pass BossBlue
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_POSWORLD();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Blue();

    }

    pass UI_InvenSlot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_UI();
        GeometryShader = compile gs_5_0 GS_UI();
        PixelShader = compile ps_5_0 PS_UI_INVENSLOT();
    }

}