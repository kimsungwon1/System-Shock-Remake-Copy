#include "Shader_Engine_Defines.hlsli"

/* float2 float3 float4 == vector */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.0f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

struct VS_IN
{
    /* InputSlot : 0 */
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;		
	float3 vTangent : TANGENT;
    
    /* InputSlot : 1 */
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : COLOR;
};

struct VS_OUT
{	
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
};

VS_OUT VS_MAIN(/*Á¤Á¡*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = mul(vPosition, matWVP);
    Out.vLifeTime = In.vLifeTime;

	return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vDiffuse.a = In.vLifeTime.y / In.vLifeTime.x;
    
	return Out;
}

technique11	DefaultTechnique
{
	pass Terrain
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}