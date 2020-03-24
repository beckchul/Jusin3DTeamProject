matrix			g_matWorld, g_matView, g_matProj;


// 색상
float4		g_vColor = float4(1.f, 1.f, 1.f, 1.f);
// UV
float2		g_vUV = float2(1.f, 1.f);


texture		g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture	= g_BaseTexture;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

struct VS_IN
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);

	Out.vTexUV = (In.vTexUV * g_vUV);

	return Out;
}

struct PS_IN
{
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= g_vColor;
	clamp(Out.vColor, 0.f, 1.f);		// 1 넘지마셈

	return Out;
}

technique Default_Technique
{

	pass Default
	{ 
		LIGHTING = FALSE;
		CULLMODE = none;

		ALPHABLENDENABLE = TRUE;		
		BLENDOP = ADD;
		SRCBLEND = SRCALPHA/*ONE*/;
		DESTBLEND = ONE;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	
};
