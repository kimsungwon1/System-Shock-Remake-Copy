
/* float2 float3 float4 == vector */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_BrushTexture;

vector g_vBrushPos = vector(0.f, 0.f, 0.f, 0.f);
float g_fBrushRange = 0.1f;

bool g_bBrush = false;

sampler LinearSampler = sampler_state 
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;		
	float3 vTangent : TANGENT;
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
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = mul(vector(In.vPosition, 1.f), matWVP);
    
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
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vPickDepth : SV_TARGET3;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    vector vBrush = vector(0.f, 0.f, 0.f, 0.f);

    if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange && g_bBrush)
    {		
        float2 vBrushTexcoord;
		
        vBrushTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vBrushTexcoord.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

        vBrush = g_BrushTexture.Sample(LinearSampler, vBrushTexcoord);
        if (vBrush.a < 1.f)
        {
            vBrush = vector(0.f, 0.f, 0.f, 0.f);
        }
    }
	
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord); //(1.f, 1.f, 1.f, 1.f); 
	
    Out.vColor = Out.vColor + vBrush;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);

	return Out;
}



technique11	DefaultTechnique
{
	pass Terrain
    {
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}