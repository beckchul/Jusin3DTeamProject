
matrix			g_matWorld, g_matView, g_matProj;

texture			g_BaseTexture;

sampler BaseSampler = sampler_state
{
	//mipfilter = linear;
	minfilter = linear;
	magfilter = linear;
	texture = g_BaseTexture;
};

texture			g_DestTexture;

sampler DestSampler = sampler_state
{
	mipfilter = linear;
	minfilter = linear;
	magfilter = linear;
	texture = g_DestTexture;
};

texture			g_FilterTexture;

sampler FilterSampler = sampler_state
{
	mipfilter = linear;
	minfilter = linear;
	magfilter = linear;

	texture = g_FilterTexture;
};

struct VS_IN
{
	vector		vPosition : POSITION;
	vector		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	vector		vNormal : NORMAL;
	vector		vProjPos : COLOR0;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0.0f;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);

	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);

	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	Out.vProjPos = Out.vPosition;

	Out.vTexUV = In.vTexUV;

	return Out;
}

// Pixel Shader

struct PS_IN
{
	vector				vNormal : NORMAL;
	vector				vProjPos : COLOR0;
	float2				vTexUV : TEXCOORD0;
};


struct PS_OUT
{
	vector				vColor : COLOR0;
	vector				vNormal : COLOR1;
	vector				vDepth : COLOR2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	vector		vColor[3];

	vColor[0] = tex2D(BaseSampler, In.vTexUV * 30.f);
	vColor[1] = tex2D(DestSampler, In.vTexUV * 30.f);
	vColor[2] = tex2D(FilterSampler, In.vTexUV);

	Out.vColor = vector(vColor[1] * vColor[2] + vColor[0] * (1.f - vColor[2]).xyz, 1.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f); // 안쓰는 4번째 인자를 bool 변수로서 활용도 가능.
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

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
}