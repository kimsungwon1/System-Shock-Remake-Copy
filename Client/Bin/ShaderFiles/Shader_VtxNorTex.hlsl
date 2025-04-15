#include "Shader_Engine_Defines.hlsli"

/* float2 float3 float4 == vector */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

texture2D		g_DiffuseTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;		
};

struct VS_OUT
{	
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(/*Á¤Á¡*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = vPosition;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;	
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f);
	
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
	
    vector vLook = In.vWorldPos - g_vCamPosition;
	
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
	
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * (fShade + (g_vLightAmbient * g_vMtrlSpecular))
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
	return Out;
}



technique11	DefaultTechnique
{
	pass Terrain
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}