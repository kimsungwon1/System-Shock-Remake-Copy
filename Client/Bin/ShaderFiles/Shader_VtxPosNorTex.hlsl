#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* 정점하나당 어떤 변환을 거쳐가면 되는지를 직접 연산을 통해 구현. */
/* 정점 쉐이더는 정점 하나를 받아오고. 연산을 거친후 연산을 마친 정점을 리턴해준다. */
/* 사각형버퍼를 그린다. 6번호출. */
// VS_MAIN(/*정점*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
texture2D       g_NormalTexture;

int g_iHP = 0;
float g_fTime;
float g_fFullTime;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;	
	
};

struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
    VS_OUT_NORMAL Out = (VS_OUT_NORMAL) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(0.f, 0.f, 1.f, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(vector(1.f, 0.f, 0.f, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(vector(0.f, 1.f, 0.f, 0.f), g_WorldMatrix));
    Out.vProjPos = vPosition;
    
    return Out;
}

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_Target2;
};

PS_OUT PS_MAIN(PS_IN_NORMAL In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

	//if (0.1 >= Out.vColor.a)
		//discard;

    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    Out.vColor.gb = Out.vColor.r;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_BOMBBLOW(PS_IN_NORMAL In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    int num = (int) (g_fTime / g_fFullTime * 64);
	
    float2 vBlowTexcoord;
	
    // 가로세로 모두 8
    int numX = num % 8;
    int numY = num / 8;
    vBlowTexcoord.x = 1.f / 8.f * (float) numX + In.vTexcoord.x / 8.f;
    vBlowTexcoord.y = 1.f / 8.f * (float) numY + In.vTexcoord.y / 8.f;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vBlowTexcoord);
    
    if(Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, vBlowTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT PS_BLOODHIT(PS_IN_NORMAL In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

	//if (0.1 >= Out.vColor.a)
		//discard;

    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    if (Out.vColor.r < 0.3f && Out.vColor.g < 0.3f && Out.vColor.b < 0.3f)
    {
        discard;
    }
    
    //Out.vColor = mul(Out.vColor, vector(1.f, 0.f, 0.f, 1.f));
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
    return Out;
}

technique11	DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
	pass EFFECT
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = null;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass BombBlow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_BOMBBLOW();
    }

    pass BloodHit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_BLOODHIT();
    }
	/* 디스토션 + 블렌딩 */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}