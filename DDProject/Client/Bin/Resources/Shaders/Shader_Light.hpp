
/* 정반사광의 세기를 계산하자.
픽셀의 위치를 구해야한다. 여기서 필요한 픽셀의 위치는 월드영역상의 위치.
UV좌표를 통해 월드영역상에 지형을 이루고 있는 픽셀이 어디에 있는지 추측할 수 있다.
현재 픽셀이 그려지고 있는 것은 백버퍼 사이즈의 크기를 갖는 임의의 버퍼에 픽셀이 그려지는 것이다.
현재 임의의 버퍼에 그려지는 픽셀들은 투영역영에서 그려진 픽셀들이다. 즉, UV좌표를 투영윈도우 상의 위치로 바꾸자.
UV좌표의 시작점 (0,0)을 (-1,1)로 (1,1)을 (1, -1)인 투영좌표로 바꾼다.(x, y위치인 투영 윈도우 상의 위치를 알 수 있다.)
이후 View영역의 z를 x,y에 다시 곱하여서 View영역 상에 투영행렬까지 곱해진 상태로 만들어 거꾸로 계산해 나간다.*/



//===================================================================

//===== Constant Table ==============================================

// For.LightInfo Directional Light
float3 g_LightDir;
float4 g_LightDiffuse = float4(1.f, 1.f, 1.f, 1.f);
float4 g_LightAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_LightSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_MtrlDiffuse = float4(1.f, 1.f, 1.f, 1.f);
float4 g_MtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_MtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_CamPosition;

float4x4 g_matViewInv;
float4x4 g_matProjInv;

float4x4 g_matViewProjInv;

float4x4 g_matLightView; // 광원 - 뷰
float4x4 g_matLightProj; // 광원 - 투영

float g_fPower = 5.f;

// For.LightInfo Point Light
float4 g_LightPosition;
float g_fRange = 0.f;
float g_BrightNess = 1.f;

float g_fFar = 0.f;

//===================================================================
//===================================================================

texture g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

texture g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

texture g_SpecularIntensityTexture;

sampler SpecSampler = sampler_state
{
	texture = g_SpecularIntensityTexture;
};

struct PS_INPUT
{
	float2 mTexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 mDiffuse : COLOR0;
	float4 mSpecular : COLOR1;
};

PS_OUTPUT PS_MAIN_DIRECTIONAL(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.f;

	/*float4 Depth = tex2D(DepthSampler, Input.mTexCoord);

	float4 PixelPosition = float4(Input.mTexCoord.x * 2 - 1, (1 - Input.mTexCoord.y) * 2 - 1, Depth.x, 1);
	float4 D = mul(PixelPosition, g_matViewProjInv);
	float4 worldPos = D / D.w;*/

	float4 Depth = tex2D(DepthSampler, Input.mTexCoord);

	//// Dpeth값 기록안한 부분의 픽셀은 조명연산도 하지 않음. 
	//if (Depth.b >= 1.f)
	//	return Output;

	float fViewZ = Depth.y * g_fFar; // View영역에서의 z깊이.

	float4 PixelPosition;

	PixelPosition.x = (Input.mTexCoord.x * 2.f - 1.f) * fViewZ;
	PixelPosition.y = (Input.mTexCoord.y * -2.f + 1.f) * fViewZ;
	PixelPosition.z = Depth.x / fViewZ; // 깊이타겟의 x는 투영행렬 z나누기가 된 값이므로 다시 z값을 곱한다.
	PixelPosition.w = fViewZ;

	PixelPosition = mul(PixelPosition, g_matViewProjInv);

	PixelPosition.xyz = PixelPosition.xyz / PixelPosition.w;

	float4 Normal = tex2D(NormalSampler, Input.mTexCoord);
	Normal = float4(Normal.xyz * 2.f - 1.f, 0.f);

	// 난반사광 구하기 (램버트 확산공식)
	float Diffuse = saturate(dot(normalize(-g_LightDir), Normal.xyz)); // 난반사광의 양을 구한다.
	Diffuse = ceil(Diffuse * 5) / 5.0f; // 카툰렌더링을 위해서 난반사광의 양을 0.2단위로 잘라낸다.

	// Ambient + Diffuse
	Output.mDiffuse = (g_LightAmbient * g_MtrlAmbient) + (Diffuse * (g_LightDiffuse * g_MtrlDiffuse)) + Depth.w;
	Output.mDiffuse *= g_BrightNess;

	float4 fSpecular = 0;
	if (Diffuse > 0)
	{
		// 카메라가 인게임상의 픽셀을 바라보는 방향벡터를 구한다.
		float3 CamLookDir = normalize(PixelPosition.xyz - g_CamPosition.xyz);
		// 반사벡터를 구한다.
		float3 Reflect = normalize(reflect(g_LightDir, Normal.xyz));

		fSpecular = saturate(dot(Reflect, -CamLookDir));
		fSpecular = pow(fSpecular, g_fPower);

		float4 specularIntensity = tex2D(SpecSampler, Input.mTexCoord);

		fSpecular *= specularIntensity * (g_LightSpecular * g_MtrlSpecular);
		fSpecular.a = 0.f;

		Output.mSpecular = fSpecular;
	}
	

	return Output;
}

PS_OUTPUT PS_MAIN_POINT(PS_INPUT Input)
{
	PS_OUTPUT		Output = (PS_OUTPUT)0;

	float4 Depth = tex2D(DepthSampler, Input.mTexCoord);

	// Dpeth값 기록안한 부분의 픽셀은 조명연산도 하지 않음. 
	if (Depth.b >= 1.f)
		return Output;

	float fViewZ = Depth.y * g_fFar; // View영역에서의 z깊이.

	float4 PixelPosition;

	PixelPosition.x = (Input.mTexCoord.x * 2.f - 1.f) * fViewZ;
	PixelPosition.y = (Input.mTexCoord.y * -2.f + 1.f) * fViewZ;
	PixelPosition.z = Depth.x * fViewZ; // 깊이타겟의 x는 투영행렬 z나누기가 된 값이므로 다시 z값을 곱한다.
	PixelPosition.w = fViewZ;

	PixelPosition = mul(PixelPosition, g_matViewProjInv);

	float4	LightDir = PixelPosition - g_LightPosition;
	// Normalize하기 전의 LightDir의 길이를 미리 구해놓는다.
	float	fDistance = length(LightDir);

	// 라이팅에 영향을 받는 픽셀을 찾아내서 체크하는 과정이 필요함.
	if (fDistance <= g_fRange)
	{
		float4 Normal = tex2D(NormalSampler, Input.mTexCoord);
		Normal = float4(Normal.xyz * 2.f - 1.f, 0.f);

		// 난반사광 구하기 (램버트 확산공식)
		float Diffuse = saturate(dot(normalize(-LightDir), Normal));

		float fAtt = saturate((g_fRange - fDistance) / g_fRange);
		// Ambient + Diffuse
		Output.mDiffuse = (g_LightAmbient * g_MtrlAmbient) + (Diffuse * fAtt) * (g_LightDiffuse * g_MtrlDiffuse);
		Output.mDiffuse *= g_BrightNess;

		float4 fSpecular = 0;
		if (Diffuse > 0)
		{
			// 카메라가 인게임상의 픽셀을 바라보는 방향벡터를 구한다.
			float3 CamLookDir = normalize(PixelPosition.xyz - g_CamPosition.xyz);
			// 반사벡터를 구한다.
			float3 Reflect = normalize(reflect(g_LightDir, Normal.xyz));

			fSpecular = saturate(dot(Reflect, -CamLookDir));
			fSpecular = pow(fSpecular, g_fPower);

			float4 specularIntensity = tex2D(SpecSampler, Input.mTexCoord);

			fSpecular *= specularIntensity * (g_LightSpecular * g_MtrlSpecular);
			fSpecular.a = 0.f;

			Output.mSpecular = fSpecular;
		}
	}
	// Ambient + Diffuse + Specular;
	return Output;
}

technique Default_Device
{
	pass Directional
	{
		// 점조명과 방향성 광원의 색이 합쳐지기 위해서 알파블랜딩.
		AlphaBlendEnable = true;
		SrcBlend = ONE;
		DestBlend = ONE;

		ZEnable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN_DIRECTIONAL();
	}
	pass Point
	{
		// 점조명과 방향성 광원의 색이 합쳐지기 위해서 알파블랜딩.
		AlphaBlendEnable = true;
		SrcBlend = ONE;
		DestBlend = ONE;

		ZEnable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}
}