//===================================================================

//===== Constant Table ==============================================

//float4x4 g_matWorld, g_matView, g_matProj;
float4x4 g_matWorld;
float4x4 g_matWorldViewProj;
float4x4 g_matLightView; // 광원 - 뷰
float4x4 g_matLightProj; // 광원 - 투영

//bool g_bSpecularIntensityExist; // 스펙큘러 세기가 없으면 처리하면 안된다.
//bool g_bNormalExist; // 노말이 없다면 처리방식이 달라진다.
bool g_bEmissiveExist;

float4x4 MatrixPalette[53];
int numBoneInfluences = 2;

float4 g_fColor = { 1.f, 1.f, 1.f, 1.f };

float g_fFar = 0.f;

float4 g_worldCameraPosition;

float3 g_LightColor = float3(0.3f, 0.3f, 0.3f); // Light Diffuse, Specular라고 보면됨.

float g_fBrightNess = 0.f;

float4 g_fHitColor = float4(0.f, 0.f, 0.f, 0.f);
//===================================================================
//===================================================================

texture		g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	MIPFILTER = NONE;
	MINFILTER = POINT;
	MAGFILTER = POINT;
};

texture g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

texture g_SpecularIntensityTexture;

sampler SpecSampler = sampler_state
{
	texture = g_SpecularIntensityTexture;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

texture g_EmissiveTexture;

sampler EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;
};

texture			g_ShadowTexture;

sampler ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
};

struct VS_IN
{
	float4	mPosition : POSITION;
	float4	weights : BLENDWEIGHT0;
	int4	boneIndices : BLENDINDICES0;
	float3	mNormal : NORMAL;
	float3	mTangent : TANGENT;
	float3  mBinormal : BINORMAL;
	float2	mUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 T : TEXCOORD1;
	float3 B : TEXCOORD2;
	float3 N : TEXCOORD3;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};

struct VS_BASIC_INPUT
{
	float4	mPosition : POSITION;
	float4	weights : BLENDWEIGHT0;
	int4	boneIndices : BLENDINDICES0;
	float3	mNormal : NORMAL;
	float2	mUV : TEXCOORD0;
};

struct VS_BASIC_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 N : TEXCOORD1;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};




struct VS_FORWARD_OUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 mLightDir : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};

// Pixel Shader

struct PS_IN
{
	float2 mUV : TEXCOORD0;
	float3 T : TEXCOORD1;
	float3 B : TEXCOORD2;
	float3 N : TEXCOORD3;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};

struct PS_OUT
{
	float4 mColor : COLOR0;
	float4 mNormal : COLOR1;
	float4 mDepth : COLOR2;
	float4 mSpecIntensity : COLOR3;
};


struct PS_BASIC_INPUT
{
	float2 mUV : TEXCOORD0;
	float3 N : TEXCOORD1;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};



struct PS_FORWARD_INPUT
{
	float2 mUV : TEXCOORD0;
	float3 mLightDir : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	//float4 worldPos = mul(In.mPosition, g_matWorld);

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float3 norm = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = numBoneInfluences - 1;

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.mPosition, MatrixPalette[In.boneIndices[i]]);

		norm += In.weights[i] * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[i]]);
	}

	lastWeight = 1.0f - lastWeight;
	p += lastWeight * mul(In.mPosition, MatrixPalette[In.boneIndices[n]]);

	norm += lastWeight * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[n]]);
	p.w = 1.0f;

	Out.mPosition = mul(p, g_matWorldViewProj);

	Out.N = normalize(mul(In.mNormal, (float3x3)g_matWorld));
	Out.T = normalize(mul(In.mTangent, (float3x3)g_matWorld));
	Out.B = normalize(mul(In.mBinormal, (float3x3)g_matWorld));

	Out.mUV = In.mUV;

	Out.mProjPos = Out.mPosition;

	Out.mClipPos = mul(p, g_matWorld);
	Out.mClipPos = mul(Out.mClipPos, g_matLightView);
	Out.mClipPos = mul(Out.mClipPos, g_matLightProj);

	return Out;
}

PS_OUT PS_MAIN(PS_IN Input)
{
	PS_OUT Output = (PS_OUT)0;

	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

	worldNormal = worldNormal * 0.5f + 0.5f;

	// Normal
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec0
	float4 specularIntensity = tex2D(SpecSampler, Input.mUV);
	Output.mSpecIntensity = specularIntensity;
	 
	// albedo
	float4 albedo = tex2D(BaseSampler, Input.mUV);
	Output.mColor = albedo * g_fColor;
	Output.mColor.a = 0.f;

	Output.mColor += g_fHitColor;

	if (g_bEmissiveExist)
	{
		// EmissiveTexture
		float4 emissive = tex2D(EmissiveSampler, Input.mUV);
		Output.mColor.a = emissive.r;
	}


	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 그것을 xyzw중에 y에 저장.
	// Depth
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w
		, Input.mProjPos.w / g_fFar
		, 0.f
		, g_fBrightNess);

	// 그림자 드리우기.
	float currentDepth = Input.mClipPos.z / Input.mClipPos.w;

	float2 uv = Input.mClipPos.xy / Input.mClipPos.w;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;

	float shadowDepth = tex2D(ShadowSampler, uv).r;

	if (currentDepth > shadowDepth + 0.0000125f)
	{
		Output.mColor.rgb *= 0.5f;
	}


	return Output;
}

PS_OUT PS_MAIN2(PS_IN Input)
{
	PS_OUT Output = (PS_OUT)0;

	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

	worldNormal = worldNormal * 0.5f + 0.5f;

	// Normal
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec0
	float4 specularIntensity = float4(0.f, 0.f, 0.f, 0.f);

	// albedo
	float4 albedo = tex2D(BaseSampler, Input.mUV);
	Output.mColor = albedo * g_fColor;
	Output.mColor.a = 0;

	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 그것을 xyzw중에 y에 저장.
	// Depth
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w
		, Input.mProjPos.w / g_fFar
		, 0.f
		, 0.f);

	// 그림자 드리우기.
	float currentDepth = Input.mClipPos.z / Input.mClipPos.w;

	float2 uv = Input.mClipPos.xy / Input.mClipPos.w;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;

	float shadowDepth = tex2D(ShadowSampler, uv).r;

	if (currentDepth > shadowDepth + 0.0000125f)
	{
		Output.mColor.rgb *= 0.5f;
	}


	return Output;
}



VS_BASIC_OUTPUT VS_BASIC_MAIN(VS_BASIC_INPUT In)
{
	VS_BASIC_OUTPUT	Out = (VS_BASIC_OUTPUT)0;

	//float4 worldPos = mul(In.mPosition, g_matWorld);

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float3 norm = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = numBoneInfluences - 1;

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.mPosition, MatrixPalette[In.boneIndices[i]]);

		norm += In.weights[i] * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[i]]);
	}

	lastWeight = 1.0f - lastWeight;
	p += lastWeight * mul(In.mPosition, MatrixPalette[In.boneIndices[n]]);

	norm += lastWeight * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[n]]);
	p.w = 1.0f;

	Out.mPosition = mul(p, g_matWorldViewProj);

	Out.N = normalize(mul(In.mNormal, (float3x3)g_matWorld));

	Out.mUV = In.mUV;

	Out.mProjPos = Out.mPosition;

	Out.mClipPos = mul(p, g_matWorld);
	Out.mClipPos = mul(Out.mClipPos, g_matLightView);
	Out.mClipPos = mul(Out.mClipPos, g_matLightProj);

	return Out;
}

PS_OUT PS_BASIC_MAIN(PS_BASIC_INPUT Input)
{
	PS_OUT Output = (PS_OUT)0;

	float3 worldNormal = normalize(Input.N) * 0.5f + 0.5f;

	// Normal
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec0
	float4 specularIntensity = float4(0.f, 0.f, 0.f, 0.f);

	// albedo
	float4 albedo = tex2D(BaseSampler, Input.mUV);
	Output.mColor = albedo * g_fColor;
	Output.mColor.a = 0;
	Output.mColor += g_fHitColor;

	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 그것을 xyzw중에 y에 저장.
	// Depth
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w
		, Input.mProjPos.w / g_fFar
		, 0.f
		, 0.f);

	// 그림자 드리우기.
	float currentDepth = Input.mClipPos.z / Input.mClipPos.w;

	float2 uv = Input.mClipPos.xy / Input.mClipPos.w;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;

	float shadowDepth = tex2D(ShadowSampler, uv).r;

	if (currentDepth > shadowDepth + 0.0000125f)
	{
		Output.mColor.rgb *= 0.5f;
	}


	return Output;
}



VS_FORWARD_OUT VS_TOWER(VS_IN In)
{
	VS_FORWARD_OUT	Out = (VS_FORWARD_OUT)0;

	//float4 worldPosition = mul(In.mPosition, g_matWorld);

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float3 norm = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = numBoneInfluences - 1;

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.mPosition, MatrixPalette[In.boneIndices[i]]);

		norm += In.weights[i] * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[i]]);
	}

	lastWeight = 1.0f - lastWeight;
	p += lastWeight * mul(In.mPosition, MatrixPalette[In.boneIndices[n]]);

	norm += lastWeight * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[n]]);
	p.w = 1.0f;

	Out.mPosition = mul(p, g_matWorldViewProj);
	Out.mUV = In.mUV;

	float4 worldPosition = mul(p, g_matWorld);
	
	Out.mLightDir = normalize(float3(0.f, -0.57735f, 0.f));

	float3 viewDir = normalize(worldPosition.xyz - g_worldCameraPosition.xyz);
	Out.mViewDir = viewDir;

	Out.N = normalize(mul(In.mNormal, (float3x3)g_matWorld));
	Out.T = normalize(mul(In.mTangent, (float3x3)g_matWorld));
	Out.B = normalize(mul(In.mBinormal, (float3x3)g_matWorld));

	return Out;
}

float4 PS_TOWER(PS_FORWARD_INPUT Input) : COLOR
{
	// 반투명오브젝트 이거는 포워드로 그려야함
	float3 lightColor = g_LightColor.rgb + g_fBrightNess.xxx;

	if (Input.mUV.y > g_fColor.x)
		discard;

	// Normal
	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal);

	float4 albedo = tex2D(BaseSampler, Input.mUV);
	float3 lightDir = normalize(Input.mLightDir);
	float3 diffuse = saturate(dot(worldNormal, -lightDir));
	diffuse = lightColor * albedo.rgb * diffuse;

	float3 specular = 0;
	if (diffuse.x > 0)
	{
		float3 reflection = reflect(lightDir, worldNormal);
		float3 viewDir = normalize(Input.mViewDir);

		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.0f);

		float4 specularIntensity = tex2D(SpecSampler, Input.mUV);
		specular *= specularIntensity.rgb * lightColor;
	}

	float3 ambient = float3(0.2f, 0.2f, 0.2f) * albedo.rgb;

	return float4(ambient + diffuse + specular, g_fColor.a);
}

technique Default_Technique
{
	// 1. 노말, 스펙큘러 있는경우.
	pass BumpMapping
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	// 2. 노말만 있는경우.
	pass BumpMapping_2
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN2();
	}
	// 3. 아예 없는 경우.
	pass BasicRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BASIC_MAIN();
		PixelShader = compile ps_3_0 PS_BASIC_MAIN();
	}
	
	// Forward Rendering
	pass TowerGenerating
	{
		lighting = false;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;
		
		VertexShader = compile vs_3_0 VS_TOWER();
		PixelShader = compile ps_3_0 PS_TOWER();
	}
};