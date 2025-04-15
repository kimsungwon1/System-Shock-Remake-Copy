#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* �����ϳ��� � ��ȯ�� ���İ��� �Ǵ����� ���� ������ ���� ����. */
/* ���� ���̴��� ���� �ϳ��� �޾ƿ���. ������ ��ģ�� ������ ��ģ ������ �������ش�. */
/* �簢�����۸� �׸���. 6��ȣ��. */
// VS_MAIN(/*����*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
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

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

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
	
    // ���μ��� ��� 8
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
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
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
	/* ����� + ���� */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}