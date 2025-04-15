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
texture2D       g_Texture2;

float2 g_fFrame;
float2 g_vRandPos[4];

int g_iHP = 0;
float g_fTime;
float g_fFullTime;
float3 g_arrPoints[20];

float g_fadeTime;

int g_iArrNum;

int g_iFrame = 0;


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;	
	
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/*����*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

VS_OUT VS_DOOR( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    matrix newWorldMat = g_WorldMatrix;
    newWorldMat[0] *= 2.f;
    newWorldMat[1] *= 2.f;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), newWorldMat);
    vPosition += newWorldMat._21_22_23_24 * 0.5f;
	
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_NORMAL
{
    float3 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);/*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/	

	//if (0.1 >= Out.vColor.a)
		//discard;

	//Out.vColor.gb = Out.vColor.r;

	return Out;
}

PS_OUT PS_HEALTHBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    int num = 24 - g_iHP;
	
    float2 vHealthTexcoord;
	
    if ((num) % 2 == 0)
    {
        vHealthTexcoord.x = In.vTexcoord.x * 0.5f;
    }
    else
    {
        vHealthTexcoord.x = 0.5f + In.vTexcoord.x * 0.5f;
    }
    int numY = num / 2;
    vHealthTexcoord.y = 1.f / 13.f * (float)numY + In.vTexcoord.y / 13.f;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vHealthTexcoord);
	
	if(Out.vColor.r == 0 && Out.vColor.g == 0 && Out.vColor.b == 0)
        discard;
	
    return Out;
}

PS_OUT PS_GUNFIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
    {
        discard;
    }
	
    vector c1 = (vector(1.f, 1.f, 1.f, 2.f) - Out.vColor) * vector(0.8f, 0.8f, 0.1f, 0.f);
    Out.vColor = c1 + Out.vColor;
	//Out.vColor = Out.vColor * vector(0.8f, 0.8f, 0.1f, 1.f);
    //Out.vColor.a = Out.vColor.a * (0.5f - g_fTime / 0.5f);
	
    return Out;
}

PS_OUT PS_CROSSHAIR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

	if (0.1 >= Out.vColor.a)
		discard;
    
    if(Out.vColor.r > 0.3f && Out.vColor.g > 0.3f && Out.vColor.b > 0.3f)
        discard;
    
    return Out;
}

PS_OUT PS_BULLETHIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    if (Out.vColor.r == 0 && Out.vColor.g == 0 && Out.vColor.b == 0)
        discard;

    Out.vColor = Out.vColor * vector(0.8f, 0.8f, 0.1f, 0.f);
    
    Out.vColor.a = Out.vColor.r * (1 - g_fTime / g_fFullTime);
    
    return Out;
}

PS_OUT PS_DOOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
    Out.vColor.a = g_fadeTime;
    Out.vColor.rgb = float3(0.f, 0.5f, 1.f);
    
    if(g_iFrame == 1)
    {
        Out.vColor.rgb = float3(1.f, 0.f, 0.f);
    }
    
    if (g_iFrame == 2)
    {
        Out.vColor.rgb = float3(0.f, 0.f, 0.f);
    }
    
    return Out;
}

PS_OUT PS_BLAST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vRealTexcoord;
    vRealTexcoord.x = float(g_iFrame % 2) * 0.5f + In.vTexcoord.x * 0.5f;
    
    int numY = g_iFrame / 2;
    
    vRealTexcoord.y = float(numY) * 0.5f + In.vTexcoord.y * 0.5f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vRealTexcoord);
    
    if (Out.vColor.r == 0 && Out.vColor.g == 0 && Out.vColor.b == 0)
        discard;
	
    vector c1 = (vector(1.f, 1.f, 1.f, 2.f) - Out.vColor) * vector(0.8f, 0.8f, 0.1f, 0.f);
    Out.vColor = c1 + Out.vColor;
    
    return Out;
}

PS_OUT PS_ENEMYGUNFIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    if (Out.vColor.r == 0 && Out.vColor.g == 0 && Out.vColor.b == 0)
        discard;

    Out.vColor.a = Out.vColor.r;
    
    Out.vColor.rgb = Out.vColor.rgb * float3(228.f, 209.f / 256.f, 135.f / 256.f);
    
    float fRound = (g_fFullTime * g_fFullTime - g_fTime * g_fTime) * 0.7f;
    float fDistanceFromCenter = sqrt(pow(In.vTexcoord.x - 0.5f, 2.f) + pow(In.vTexcoord.y - 0.5f, 2.f));
    if (fDistanceFromCenter < fRound * fRound)
    {
        Out.vColor.rgb += (1 - fRound * fRound) * float3(1.f, 1.f, 1.f);
        
    }
    else if (fDistanceFromCenter >= fRound)
    {
        Out.vColor.a *= Out.vColor.a;
    }
    
    
    
    return Out;
}

PS_OUT PS_MUZZLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    float2 vRealTexcoord;
    vRealTexcoord.x = float(g_iFrame % 2) * 0.5f + In.vTexcoord.x * 0.5f;
    
    int numY = g_iFrame / 2;
    
    vRealTexcoord.y = float(numY) * 0.5f + In.vTexcoord.y * 0.5f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, vRealTexcoord);
    
    //Out.vColor.a = (1 - g_fFrame.x) / g_fFrame.y;
    
    return Out;
}

PS_OUT PS_BUTTON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

	if (0.1 >= Out.vColor.a)
		discard;

	Out.vColor.gb = Out.vColor.r;

    return Out;
}

PS_OUT PS_TEXTBOX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor.rg = 0;
    
    return Out;
}

PS_OUT PS_INVENSLOT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a > 0.5f)
    {
        Out.vColor.a = 0.5f; // = (0.f, 1.f, 0.f, 0.5f);
    }
    
    return Out;
}

technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
	pass UI
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass HealthUI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_HEALTHBAR();
    }

    pass GunFire
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_GUNFIRE();
    }

    pass Crosshair
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_CROSSHAIR();
    }

    pass BulletHit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_BULLETHIT();
    }

    pass Door
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_DOOR();
        PixelShader = compile ps_5_0 PS_DOOR();
    }
    
    pass Blast
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_BLAST();
    }

    pass EnemyGunFire
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_ENEMYGUNFIRE();
    }

    pass Muzzle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_MUZZLE();
    }

    pass PS_BUTTON
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_BUTTON();
    }

    pass PS_TEXTBOX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_TEXTBOX();
    }

    pass PS_Inven
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = null;
        PixelShader = compile ps_5_0 PS_INVENSLOT();

    }

}