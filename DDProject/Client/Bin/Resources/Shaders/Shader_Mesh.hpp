float4x4 g_matWorld, g_matView, g_matProj;

texture g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	MIPFILTER = LINEAR;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	texture = g_DiffuseTexture;
};

texture g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

texture g_SpecularIntensityTexture;

sampler SpecSampler = sampler_state
{
	texture = g_SpecularIntensityTexture;
};

//float3 g_LightDir;
//float4 g_CamPosition;

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
	float4	mPosition : POSITION;
	float2	mUV : TEXCOORD0;
	/*float3  mLightDir : TEXCOORD1;
	float3	mViewDir : TEXCOORD2;*/
	float3  T : TEXCOORD3;
	float3  B : TEXCOORD4;
	float3  N : TEXCOORD5;
	float4	mProjPos : COLOR0;
};

VS_OUTPUT VS_MAIN(VS_INPUT Input)
{
	VS_OUTPUT Output;

	Output.mPosition = mul(Input.mPosition, g_matWorld);

	/*float4 worldPosition = mul(Input.mPosition, g_matWorld);
	Output.mLightDir = normalize(g_LightDir);

	float3 viewDir = normalize(worldPosition.xyz - g_CamPosition.xyz);
	Output.mViewDir = viewDir;*/

	Output.mPosition = mul(Output.mPosition, g_matView);
	Output.mPosition = mul(Output.mPosition, g_matProj);

	Output.N = normalize(mul(Input.mNormal, (float3x3)g_matWorld));
	Output.T = normalize(mul(Input.mTangent, (float3x3)g_matWorld));
	Output.B = normalize(mul(Input.mBinormal, (float3x3)g_matWorld));

	Output.mUV = Input.mUV;

	Output.mProjPos = Output.mPosition;

	return Output;
}

struct PS_INPUT
{
	float2	mUV : TEXCOORD0;
	/*float3  mLightDir : TEXCOORD1;
	float3  mViewDir : TEXCOORD2;*/
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
	float4	mProjPos : COLOR0;
};

struct PS_OUTPUT
{
	float4 mColor : COLOR0;
	float4 mNormal : COLOR1;
	float4 mDepth : COLOR2;
	float4 mSpecIntensity : COLOR3;
};


PS_OUTPUT PS_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output;

	float3 tangentNormal = tex2D(NormalSampler, Input.mUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal);

	Output.mColor = tex2D(DiffuseSampler, Input.mUV);

	Output.mNormal = float4(worldNormal, 0.f);

	float4 specularIntensity = tex2D(SpecSampler, Input.mUV);
	Output.mSpecIntensity = specularIntensity;

	// View영역상의 z값을 구하기 위해서 투영변환이 완료된 시점에서 View영역상에 보관된 z값(w)을 0 ~ 1사이로 표현하기 위해서 Far값으로 나눈다. 그것을 xyzw중에 y에 저장.
	Output.mDepth = float4(Input.mProjPos.z / Input.mProjPos.w, Input.mProjPos.w / 1000.f, 0.f, 0.f);

	return Output;
}

technique Default_Device
{
	pass Default_Rendering
	{
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Alphatest_Rendering
	{
		lighting = false;

		Alphatestenable = true;
		alphafunc = greater;
		alpharef = 0x7f;
		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}