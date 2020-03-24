matrix			g_matWorld, g_matView, g_matProj;

float4x4 MatrixPalette[50];
int numBoneInfluences = 2;
float	fLine;

texture		g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};

struct VS_IN
{
	vector		vPosition : POSITION;
	float4		weights : BLENDWEIGHT0;
	int4		boneIndices : BLENDINDICES0;
	vector		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	vector		vNormal : NORMAL;
	vector		vProjPos : COLOR0;
	float2		vTexUV : TEXCOORD0;

	vector		vLocalPos : TEXCOORD1; // 상,하체 분리용
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float3 norm = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = numBoneInfluences - 1;
	//In.vNormal = normalize(In.vNormal);

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.vPosition, MatrixPalette[In.boneIndices[i]]);

		norm += In.weights[i] * mul(In.vNormal, MatrixPalette[In.boneIndices[i]]);
	}

	lastWeight = 1.0f - lastWeight;
	p += lastWeight * mul(In.vPosition, MatrixPalette[In.boneIndices[n]]);

	norm += lastWeight * mul(In.vNormal, MatrixPalette[In.boneIndices[n]]);
	p.w = 1.0f;


	float4 posWorld = mul(p, g_matWorld);
	Out.vPosition = mul(posWorld, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	Out.vProjPos = Out.vPosition;

	Out.vTexUV = In.vTexUV;

	Out.vLocalPos = In.vPosition; // 상,하체 분리용

	return Out;
}

// Pixel Shader

struct PS_IN
{
	vector				vNormal : NORMAL;
	vector				vProjPos : COLOR0;
	float2				vTexUV : TEXCOORD0;

	vector				vLocalPos : TEXCOORD1; // 상,하체 분리용
};

struct PS_OUT
{
	vector		vColor : COLOR0;

	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
};

PS_OUT PS_Higher(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.0f;

	if (In.vLocalPos.z <= fLine) // 하체 분리용
		discard;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);


	return Out;
}

PS_OUT PS_Lower(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.0f;

	if (In.vLocalPos.z > fLine + 0.1f) // 상체 분리용
		discard;
	
	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

technique Default_Technique
{
	pass Higher
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Higher();
	}

	pass Lower
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Lower();
	}
};