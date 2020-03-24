//===================================================================

//===== Constant Table ==============================================

//float4x4 g_matWorld, g_matView, g_matProj;
float4x4 g_matWorld;
float4x4 g_matWorldViewProj;

float4x4 g_matLightView; // 광원 - 뷰
float4x4 g_matLightProj; // 광원 - 투영

//bool g_bSpecularIntensityExist; // 스펙큘러 세기가 없으면 처리하면 안된다.
//bool g_bNormalExist; // 노말이 없다면 처리방식이 달라진다.
//bool g_bEmissiveExist;
//bool g_bMaskExist;

float gTime = 0; // 시간값을 전달받아서 UV애니메이션 처리.
float g_fFar = 0.f;

float fbumpNess = 0.03f;
float fFlowSpeed = 0.024f;

float glowNess = 1.f;

float4 g_WorldCamPosition;

float4 g_CrystalTorchColor;

//===================================================================


texture g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	
	//// 카툰렌더링에서는 부드러운 음영변환을 원치 않기 때문에 텍스처 필터링을 하지 않는다.
	//MIPFILTER = NONE;
	//MINFILTER = POINT;
	//MAGFILTER = POINT;

	/*maxmiplevel = 0;
	MIPMAPLODBIAS = -5.f;*/

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	//MipFilter = LINEAR;
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

texture g_MaskTexture;

sampler MaskSampler = sampler_state
{
	texture = g_MaskTexture;
};



texture			g_ShadowTexture;

sampler ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
};


texture			g_LavaAlphaTexture;

sampler LavaAlphaSampler = sampler_state
{
	texture = g_LavaAlphaTexture;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

texture			g_LavaNoiseNormalTexture;

sampler LavaNoiseNormalSampler = sampler_state
{
	texture = g_LavaNoiseNormalTexture;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

texture			g_LavaNoiseTexture;

sampler LavaNoiseSampler = sampler_state
{
	texture = g_LavaNoiseTexture;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

texture			EnvironmentMap_Tex;

sampler EnvironmentSampler = sampler_state
{
	texture = EnvironmentMap_Tex;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

texture			g_RainNormalTexture;

sampler RainNormalSampler = sampler_state
{
	texture = g_RainNormalTexture;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

texture			g_CloudTexture;

sampler CloudSampler = sampler_state
{
	texture = g_CloudTexture;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};




// -------------------------------------------------------------
// 좌표변환
// -------------------------------------------------------------
float2 Q(float3 v)
{
	float2 Out = (float2)0;

	float pv = 7.0f / 8.0f;
	float ar = 1.0 / (2.0*(1.000000001 + v.y));

	Out.x = pv * ar * v.x + 0.5;
	Out.y = pv * ar * v.z + 0.5;

	return Out;
}


// -------------------------------------------------------------
// 한 옥타브의 노이즈 함수
// -------------------------------------------------------------
float noise(float gap, float2 pos)
{
	float2 x = gap * pos;
	float2 ix = floor(x) / 1024.f;
	float2 fx = frac(x);

	float x00 = tex2D(LavaNoiseSampler, ix + float2(0.f / 1024.f, 0.f / 1024.f)).x;
	float x10 = tex2D(LavaNoiseSampler, ix + float2(1.f / 1024.f, 0.f / 1024.f)).x;
	float x01 = tex2D(LavaNoiseSampler, ix + float2(0.f / 1024.f, 1.f / 1024.f)).x;
	float x11 = tex2D(LavaNoiseSampler, ix + float2(1.f / 1024.f, 1.f / 1024.f)).x;

	return lerp(lerp(x00, x01, fx.y), lerp(x10, x11, fx.y), fx.x);
}

struct VS_INPUT
{
	float4	mPosition : POSITION;
	float3  mNormal : NORMAL;
	float3	mTangent : TANGENT;
	float3  mBinormal : BINORMAL;
	float2	mUV : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};

struct VS_BASIC_INPUT
{
	float4	mPosition : POSITION;
	float3  mNormal : NORMAL;
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





struct PS_INPUT
{
	float2 mUV : TEXCOORD0;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};

struct PS_OUTPUT
{
	float4 mColor : COLOR0;
	float4 mNormal : COLOR1;
	float4 mDepth : COLOR2;
	float4 mSpecIntensity : COLOR3;
};

struct PS_BASIC_INPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 N : TEXCOORD1;
	float4 mProjPos : COLOR0;
	float4 mClipPos : COLOR1;
};





struct VS_POSITION_UV_VIEWDIR_NORMAL_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 mNormal : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
};

struct PS_POSITION_UV_VIEWDIR_NORMAL_INPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 mNormal : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
};


struct VS_POSITION_UV_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
};

struct PS_POSITION_UV_INPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
};



struct VS_FORWARD_INPUT
{
	float4 mPosition : POSITION;
	float3 mNormal : NORMAL;
	float2 mUV : TEXCOORD0;
};

struct VS_FORWARD_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 mDiffuse : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 mReflection : TEXCOORD3;
};

struct PS_FORWARD_INPUT
{
	float2 mUV : TEXCOORD0;
	float3 mDiffuse : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 mReflection : TEXCOORD3;
};



VS_OUTPUT VS_BUMP_MAIN(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.mPosition = mul(Input.mPosition, g_matWorldViewProj);

	float4 worldPos = mul(Input.mPosition, g_matWorld);

	Output.N = normalize(mul(Input.mNormal, (float3x3)g_matWorld));
	Output.T = normalize(mul(Input.mTangent, (float3x3)g_matWorld));
	Output.B = normalize(mul(Input.mBinormal, (float3x3)g_matWorld));

	Output.mUV = Input.mUV;

	Output.mProjPos = Output.mPosition;

	Output.mClipPos = mul(worldPos, g_matLightView);
	Output.mClipPos = mul(Output.mClipPos, g_matLightProj);

	return Output;
}

PS_OUTPUT PS_BUMP_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output;

	// albedo
	Output.mColor = tex2D(DiffuseSampler, Input.mUV);
	/*float4 emissive = tex2D(EmissiveSampler, Input.mUV);
	Output.mColor.a = emissive.r;*/
	Output.mColor.a = 0.f;

	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

	worldNormal = worldNormal * 0.5f + 0.5f;

	// Normal의 z에는 emissive값을 넣어놓는다.
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec
	Output.mSpecIntensity = tex2D(SpecSampler, Input.mUV);


	// 픽셀에 depth를 저장.
	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 
	Output.mDepth = float4(
		  Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
		, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
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

PS_OUTPUT PS_BUMP_MAIN2(PS_INPUT Input)
{
	PS_OUTPUT Output;

	// albedo
	Output.mColor = tex2D(DiffuseSampler, Input.mUV);
	/*float4 emissive = tex2D(EmissiveSampler, Input.mUV);
	Output.mColor.a = emissive.r;*/
	Output.mColor.a = 0.f;


	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

	worldNormal = worldNormal * 0.5f + 0.5f;

	// Normal의 z에는 emissive값을 넣어놓는다.
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	// 픽셀에 depth를 저장.
	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
		, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
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



VS_BASIC_OUTPUT VS_BASIC_MAIN(VS_BASIC_INPUT Input)
{
	VS_BASIC_OUTPUT Output = (VS_BASIC_OUTPUT)0;

	Output.mPosition = mul(Input.mPosition, g_matWorldViewProj);

	float4 worldPos = mul(Input.mPosition, g_matWorld);

	Output.N = normalize(mul(Input.mNormal, (float3x3)g_matWorld));

	Output.mUV = Input.mUV;

	Output.mProjPos = Output.mPosition;

	Output.mClipPos = mul(worldPos, g_matLightView);
	Output.mClipPos = mul(Output.mClipPos, g_matLightProj);

	return Output;
}

PS_OUTPUT PS_BASIC_MAIN(PS_BASIC_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float3 worldNormal = Input.N * 0.5f + 0.5f;

	// albedo
	Output.mColor = tex2D(DiffuseSampler, Input.mUV);
	Output.mColor.a = 0.f; // Glow 효과를 보는 녀석들만 알파값이 있어야 한다.

	// Normal
	// 픽셀에 world영역의 normal을 저장.
	Output.mNormal = float4(worldNormal, 0.f);

	// Spec
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);
	
	// Depth
	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 그것을 xyzw중에 y에 저장.
	Output.mDepth = float4(Input.mProjPos.z / Input.mProjPos.w, Input.mProjPos.w / g_fFar, 0.f, 0.f);


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



VS_POSITION_UV_VIEWDIR_NORMAL_OUTPUT VS_MAIN_CRYSTAL_MAIN(float4 mPosition : POSITION, float2 mUV : TEXCOORD0)
{
	VS_POSITION_UV_VIEWDIR_NORMAL_OUTPUT Output = (VS_POSITION_UV_VIEWDIR_NORMAL_OUTPUT)0;

	Output.mPosition = mul(mPosition, g_matWorldViewProj);

	Output.mUV = mUV;

	float4 worldPos = mul(mPosition, g_matWorld);

	float3 viewDir = normalize(worldPos.xyz - g_WorldCamPosition.xyz);
	Output.mViewDir = viewDir;

	return Output;
}

PS_OUTPUT PS_MAIN_CRYSTAL_MAIN(PS_POSITION_UV_VIEWDIR_NORMAL_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float3 viewDir = normalize(Input.mViewDir);
	float3 worldNormal = Input.mNormal * 0.5f + 0.5f;
	float3 viewReflect = reflect(viewDir, worldNormal);

	float4 albedo;

	float4 glowSource = tex2D(DiffuseSampler, Input.mUV);
	albedo.rgb = glowSource.rgb *  /*float3(0.81f, 0.698f, 1.0f)*/ float3(0.64f, 0.4f, 1.0f) * 2.0;


	float4 mask = tex2D(MaskSampler, Input.mUV);
	float4 src = glowSource * mask;

	albedo.a = src.r * 0.5f;

	float3 environment = texCUBE(EnvironmentSampler, viewReflect).rgb;

	albedo.rgb = albedo.rgb + (environment * 0.5f);
	// albedo
	Output.mColor = albedo;
	Output.mNormal = float4(1.f, 1.f, 1.f, 0.f);
	Output.mDepth = float4(1.f, 1.f, 1.f, 0.f);
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	return Output;
}



VS_POSITION_UV_OUTPUT VS_POSITION_UV_MAIN(float4 mPosition : POSITION, float2 mUV : TEXCOORD0)
{
	VS_POSITION_UV_OUTPUT Output = (VS_POSITION_UV_OUTPUT)0;

	Output.mPosition = mul(mPosition, g_matWorldViewProj);

	Output.mUV = mUV;

	return Output;
}


PS_OUTPUT PS_LAVA(PS_POSITION_UV_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	///법선맵 교란0
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	float2 uv = float2(Input.mUV.x + gTime * fFlowSpeed, Input.mUV.y); // 0.24 => flowSpeed
	float3 normal = tex2D(LavaNoiseNormalSampler, uv).rgb;

	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	float n = 0.5000f * noise(64.f, Input.mUV)
		+ 0.2500f * noise(128.f, Input.mUV)
		+ 0.1250f * noise(256.f, Input.mUV)
		+ 0.0625f * noise(512.f, Input.mUV);

	// albedo
	Output.mColor = tex2D(DiffuseSampler, float2(0.5f, n + gTime * fFlowSpeed) + fbumpNess * normal.xy);
	///Output.mColor = tex2D(DiffuseSampler, fbumpNess * normal.xy); // 그냥 물처럼 흐르기.

	// lava glow sources
	float4 lavaA = tex2D(LavaAlphaSampler/*LavaNoiseNormalSampler*/, float2(0.5f, n + gTime * fFlowSpeed) + fbumpNess * normal.xy);

	// albedo의 alpha채널에 glow sources를 저장.
	Output.mColor.a = lavaA.r;
	Output.mNormal = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mDepth = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	return Output;
}

PS_OUTPUT PS_CRYSTAL_TORCH_MAIN(PS_POSITION_UV_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 albedo;

	if (Input.mUV.x < 0.77f)
	{
		albedo = g_CrystalTorchColor;
		albedo.a = 0.8f;
	}
	else
	{
		albedo = tex2D(DiffuseSampler, Input.mUV);
		albedo.a = 0.f;
	}

	// albedo
	Output.mColor = albedo;
	Output.mNormal = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mDepth = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	return Output;
}

PS_OUTPUT PS_MAIN_CRYSTALACC_MAIN(PS_BASIC_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 albedo;

	float4 glowSource = tex2D(DiffuseSampler, Input.mUV);
	albedo.rgb = glowSource.rgb * float3(1.f, 0.8f, 0.f) * 1.88;

	albedo.a = 0.f;


	// albedo
	Output.mColor = albedo;

	// Normal
	float3 worldNormal = normalize(Input.N) * 0.5f + 0.5f;
	Output.mNormal = float4(worldNormal, 0.f);

	// Depth
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
		, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
		, 0.f
		, 0.f);

	// Spec
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	return Output;
}

PS_OUTPUT PS_PAD_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output;

	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

	worldNormal = worldNormal * 0.5f + 0.5f;

	// albedo
	float4 albedo = tex2D(DiffuseSampler, Input.mUV);
	float4 Emissive = tex2D(EmissiveSampler, Input.mUV);

	float4 color = albedo * Emissive;

	Output.mColor = albedo + color;

	// 픽셀에 world영역의 normal을 저장.
	Output.mNormal = float4(worldNormal, 0.f);
	// Normal의 z에는 emissive값을 넣어놓는다.

	float4 specularIntensity = tex2D(SpecSampler, Input.mUV);
	Output.mSpecIntensity = specularIntensity;

	// 픽셀에 depth를 저장.
	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 
	Output.mDepth = float4(
		Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
		, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
		, 0.f
		, 0.f);

	Output.mColor.a = 0.f;

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

PS_OUTPUT PS_GLOW_MAIN(PS_POSITION_UV_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float4 albedo = tex2D(DiffuseSampler, Input.mUV);
	albedo.a = glowNess;

	// albedo의 alpha채널에 glow sources를 저장.
	Output.mColor = albedo;
	Output.mNormal = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mDepth = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	Output.mSpecIntensity = float4(0.f, 0.f, 0.f, 0.f);

	return Output;
}



VS_FORWARD_OUTPUT VS_FORWARD_MAIN(VS_FORWARD_INPUT Input)
{
	VS_FORWARD_OUTPUT Output = (VS_FORWARD_OUTPUT)0;

	float4 worldPos = mul(Input.mPosition, g_matWorld);

	Output.mPosition = mul(Input.mPosition, g_matWorldViewProj);

	Output.mUV = Input.mUV;

	/*float3 lightDir = float3(0.f, -0.57735f, 0.f);
	lightDir = normalize(lightDir);

	float3 viewDir = normalize(worldPos - g_WorldCamPosition.xyz);
	Output.mViewDir = viewDir;

	float3 worldNormal = mul(Input.mNormal, (float3x3)g_matWorld);
	worldNormal = normalize(worldNormal);

	Output.mDiffuse = dot(-lightDir, worldNormal);
	Output.mReflection = reflect(lightDir, worldNormal);*/

	return Output;
}

float4 PS_FORWARD_MAIN(PS_FORWARD_INPUT Input) : COLOR0
{
	//// 법선맵 교란
	////Output.mColor = tex2D(DiffuseSampler, fbumpNess * normal.xy); // 그냥 물처럼 흐르기.

	//float2 uv = float2(Input.mUV.x, Input.mUV.y + gTime * 0.24f); // 0.24 => flowSpeed
	//float3 normal = tex2D(RainNormalSampler, uv).rgb;


	// 외곽선 아웃라인 따오기
	float4 albedo = tex2D(DiffuseSampler, Input.mUV).ggga;


	// 초록색 구름만들기 + 법선맵 교란
	float4 basicColor = float4(0.1f, 0.85f, 0.08f, 1.f);

	float2 uv = Input.mUV + float2(0.f, gTime * 0.75f);
	float4 cloud = tex2D(CloudSampler, uv);

	cloud *= basicColor;


	//// 외곽라인 안쪽
	//if (albedo.g < 0.2862745f)
	//{
	//	albedo = cloud;
	//	albedo.a = 0.75f;
	//}
	//else // 외곽라인
	//{
	//	albedo.a = 1.f;
	//}

	return albedo + cloud;

	/*float3 diffuse = saturate(Input.mDiffuse);

	diffuse = albedo * diffuse;

	float3 reflection = normalize(Input.mReflection);
	float3 viewDir = normalize(Input.mViewDir);
	float3 specular = 0;
	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.0f);
	}

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;

	return float4(ambient + diffuse + specular, albedo.a);*/
}



PS_OUTPUT PS_WALLORB_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	// 특정부위에 대해서만 빛계산하고 그 나머지 부분은 안할거임
	// 노말로 판단하는게 나을듯

	// albedo
	float4 albedo = tex2D(DiffuseSampler, Input.mUV);


	float3 normal = tex2D(NormalSampler, Input.mUV).xyz;

	if (normal.z == 1.f)
	{
		// 이부분은 normal이나 depth가 1임. 색깔입혀줘야댐
		//Output.mColor = float4(0.1411f, 0.4705f, 1.f, 0.8f);
		Output.mColor = float4(0.4941f, 0.8235f, 1.f, 0.6f);
		Output.mNormal = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
		Output.mDepth = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	}
	else
	{
		float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
		tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

		float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
		TBN = transpose(TBN);
		float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

		worldNormal = worldNormal * 0.5f + 0.5f;


		// 그렇지 않은경우에 대해서는 빛계산처리 할거임.
		Output.mColor = albedo;
		Output.mNormal = float4(worldNormal, 0.f);
		float4 specularIntensity = tex2D(SpecSampler, Input.mUV);
		Output.mSpecIntensity = specularIntensity;

		// 픽셀에 depth를 저장.
		// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 
		Output.mDepth = float4(
			Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
			, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
			, 0.f
			, 0.f);

		Output.mColor.a = 0.f;

		float currentDepth = Input.mClipPos.z / Input.mClipPos.w;

		float2 uv = Input.mClipPos.xy / Input.mClipPos.w;
		uv.y = -uv.y;
		uv = uv * 0.5 + 0.5;

		float shadowDepth = tex2D(ShadowSampler, uv).r;

		if (currentDepth > shadowDepth + 0.0000125f)
		{
			Output.mColor.rgb *= 0.5f;
		}
	}

	return Output;
}


PS_OUTPUT PS_LAMP_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	// 특정부위에 대해서만 빛계산하고 그 나머지 부분은 안할거임
	// 노말로 판단하는게 나을듯

	// albedo
	float4 albedo = tex2D(DiffuseSampler, Input.mUV);


	float3 normal = tex2D(NormalSampler, Input.mUV).xyz;

	if (normal.z == 1.f)
	{
		// 이부분은 normal이나 depth가 1임. 색깔입혀줘야댐
		Output.mColor = albedo;
		Output.mColor.a = 1.f;
		Output.mNormal = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
		Output.mDepth = float4(1.f, 1.f, 1.f, 0.f); // 빛계산 X
	}
	else
	{
		float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
		tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

		float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
		TBN = transpose(TBN);
		float3 worldNormal = mul(TBN, tangentNormal); // 이 값을 0 ~ 1사이로 다시 변환해야 한다.

		worldNormal = worldNormal * 0.5f + 0.5f;


		// 그렇지 않은경우에 대해서는 빛계산처리 할거임.
		Output.mColor = albedo;
		Output.mNormal = float4(worldNormal, 0.f);

		// 픽셀에 depth를 저장.
		// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 
		Output.mDepth = float4(
			Input.mProjPos.z / Input.mProjPos.w // 투영영역에서의 깊이
			, Input.mProjPos.w / g_fFar // 뷰영역에서의 깊이
			, 0.f
			, 0.f);

		Output.mColor.a = 0.f;

		float currentDepth = Input.mClipPos.z / Input.mClipPos.w;

		float2 uv = Input.mClipPos.xy / Input.mClipPos.w;
		uv.y = -uv.y;
		uv = uv * 0.5 + 0.5;

		float shadowDepth = tex2D(ShadowSampler, uv).r;

		if (currentDepth > shadowDepth + 0.0000125f)
		{
			Output.mColor.rgb *= 0.5f;
		}
	}

	return Output;
}

float4 PS_DEPTH_MAIN(PS_BASIC_INPUT Input) : COLOR0
{
	PS_BASIC_INPUT Output = (PS_BASIC_INPUT)0;

	float Lineardepth = Input.mProjPos.w / g_fFar;
	return float4(Lineardepth.xxx, 0.f);
}


technique Default_Device
{
	// 1. 노말, 스펙큘러 있는 경우.
	pass BumpMapping
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_BUMP_MAIN();
	}
	// 2. 노말만 있는경우.
	pass BumpMapping_2
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_BUMP_MAIN2();
	}
	// 3. 아예 없는 경우.
	pass BasicRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BASIC_MAIN();
		PixelShader = compile ps_3_0 PS_BASIC_MAIN();
	}

	pass CrystalRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_POSITION_UV_MAIN();
		PixelShader = compile ps_3_0 PS_CRYSTAL_TORCH_MAIN();
	}

	pass MainCrystalRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_MAIN_CRYSTAL_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_CRYSTAL_MAIN();
	}

	pass MainCrystalAccRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BASIC_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_CRYSTALACC_MAIN();
	}

	pass ArcanePadRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_PAD_MAIN();
	}

	pass LavaRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_POSITION_UV_MAIN();
		PixelShader = compile ps_3_0 PS_LAVA();
	}

	pass ManaCrystalRender // Forward
	{
		lighting = false;

		ALPHABLENDENABLE = TRUE;
		/*SRCBLEND = SRCALPHA;
		DESTBLEND = INVSRCALPHA;*/
		SRCBLEND = SRCCOLOR;
		DESTBLEND = ONE;

		VertexShader = compile vs_3_0 VS_FORWARD_MAIN();
		PixelShader = compile ps_3_0 PS_FORWARD_MAIN();
	}
	pass WallOrbRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_WALLORB_MAIN();
	}
	pass CanversRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_POSITION_UV_MAIN();
		PixelShader = compile ps_3_0 PS_GLOW_MAIN();
	}
	pass LampRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_LAMP_MAIN();
	}
	pass DepthRender
	{
		lighting = false;

		ALPHABLENDENABLE = FALSE;

		VertexShader = compile vs_3_0 VS_BASIC_MAIN();
		PixelShader = compile ps_3_0 PS_DEPTH_MAIN();
	}
	/*pass Alphatest_Rendering
	{
		lighting = false;

		Alphatestenable = true;
		alphafunc = greater;
		alpharef = 0x7f;
		cullmode = none;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_BUMP_MAIN();
	}

	pass BadComRender
	{
		lighting = false;

		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_BADCOM();
	}*/
}