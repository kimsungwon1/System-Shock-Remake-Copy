#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

struct VS_IN
{
    /* InputSlot : 0 */
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    /*InputSlot : 1 */
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    /*SV_ : ShaderValue*/
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    vPosition = mul(vPosition, g_WorldMatrix);
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

vector Sample(float2 vTexcoord)
{
    return g_Texture.Sample(LinearSampler, vTexcoord);
}

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
	// Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);/*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/	
    Out.vColor = Sample(In.vTexcoord);
    
    if(Out.vColor.a <= 0.3f)
        discard;

    Out.vColor.gb = Out.vColor.r;

    Out.vColor.a = Out.vColor.a * 0.5f;

    return Out;
}



technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	/* 디스토션 + 블렌딩 */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}