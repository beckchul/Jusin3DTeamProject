
matrix			g_matWorld, g_matView, g_matProj;
float			g_fSelectX = 0.f;
float			g_fSelectY = 0.f;

texture			g_BaseTexture;

sampler BaseSampler = sampler_state
{
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	texture = g_BaseTexture;
};

struct VS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0.0f;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);

	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);

	return Out;
}

// Pixel Shader

struct PS_IN
{
	float2				vTexUV : TEXCOORD0;
};

struct Select_PS_IN
{
	vector				vWorldPos : TEXCOORD1;
	float2				vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector				vColor : COLOR0;
};

PS_OUT PS_Select_MAIN(Select_PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	//if (In.vWorldPos.x < -0.6f)
	//	Out.vColor.a = 0.0f;
	
	// -1 ~ -0.5 , -0.5 ~ -0, -0 ~ 0.5, 0.5 ~ 1
	if (In.vWorldPos.x < g_fSelectX && In.vWorldPos.x > g_fSelectX - 0.5f)
	{
		if (In.vWorldPos.y < g_fSelectY && In.vWorldPos.y > g_fSelectY - 1.f)
			Out.vColor.a = 0.0f;
	}
		
	
	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device
{
	pass Default_Rendering	
	{	
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Select_Rendering
	{
		lighting = false;

		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Select_MAIN();
	}
}