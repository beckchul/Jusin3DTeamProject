
/* ���ݻ籤�� ���⸦ �������.
�ȼ��� ��ġ�� ���ؾ��Ѵ�. ���⼭ �ʿ��� �ȼ��� ��ġ�� ���念������ ��ġ.
UV��ǥ�� ���� ���念���� ������ �̷�� �ִ� �ȼ��� ��� �ִ��� ������ �� �ִ�.
���� �ȼ��� �׷����� �ִ� ���� ����� �������� ũ�⸦ ���� ������ ���ۿ� �ȼ��� �׷����� ���̴�.
���� ������ ���ۿ� �׷����� �ȼ����� ������������ �׷��� �ȼ����̴�. ��, UV��ǥ�� ���������� ���� ��ġ�� �ٲ���.
UV��ǥ�� ������ (0,0)�� (-1,1)�� (1,1)�� (1, -1)�� ������ǥ�� �ٲ۴�.(x, y��ġ�� ���� ������ ���� ��ġ�� �� �� �ִ�.)
���� View������ z�� x,y�� �ٽ� ���Ͽ��� View���� �� ������ı��� ������ ���·� ����� �Ųٷ� ����� ������.*/



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

float4x4 g_matLightView; // ���� - ��
float4x4 g_matLightProj; // ���� - ����

float g_fPower = 20.f;

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

	//// Dpeth�� ��Ͼ��� �κ��� �ȼ��� �����굵 ���� ����. 
	//if (Depth.b >= 1.f)
	//	return Output;

	float fViewZ = Depth.y * g_fFar; // View���������� z����.

	float4 PixelPosition;

	PixelPosition.x = (Input.mTexCoord.x * 2.f - 1.f) * fViewZ;
	PixelPosition.y = (Input.mTexCoord.y * -2.f + 1.f) * fViewZ;
	PixelPosition.z = Depth.x / fViewZ; // ����Ÿ���� x�� ������� z�����Ⱑ �� ���̹Ƿ� �ٽ� z���� ���Ѵ�.
	PixelPosition.w = fViewZ;

	PixelPosition = mul(PixelPosition, g_matViewProjInv);

	PixelPosition.xyz = PixelPosition.xyz / PixelPosition.w;

	float4 Normal = tex2D(NormalSampler, Input.mTexCoord);
	Normal = float4(Normal.xyz * 2.f - 1.f, 0.f);

	// ���ݻ籤 ���ϱ� (����Ʈ Ȯ�����)
	float Diffuse = saturate(dot(normalize(-g_LightDir), Normal.xyz)); // ���ݻ籤�� ���� ���Ѵ�.
	Diffuse = ceil(Diffuse * 5) / 5.0f; // ī���������� ���ؼ� ���ݻ籤�� ���� 0.2������ �߶󳽴�.

	// Ambient + Diffuse
	Output.mDiffuse = (g_LightAmbient * g_MtrlAmbient) + (Diffuse * (g_LightDiffuse * g_MtrlDiffuse)) + Depth.w;
	Output.mDiffuse *= g_BrightNess;

	float4 fSpecular = 0;
	if (Diffuse > 0)
	{
		// ī�޶� �ΰ��ӻ��� �ȼ��� �ٶ󺸴� ���⺤�͸� ���Ѵ�.
		float3 CamLookDir = normalize(PixelPosition.xyz - g_CamPosition.xyz);
		// �ݻ纤�͸� ���Ѵ�.
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

	// Dpeth�� ��Ͼ��� �κ��� �ȼ��� �����굵 ���� ����. 
	if (Depth.b >= 1.f)
		return Output;

	float fViewZ = Depth.y * g_fFar; // View���������� z����.

	float4 PixelPosition;

	PixelPosition.x = (Input.mTexCoord.x * 2.f - 1.f) * fViewZ;
	PixelPosition.y = (Input.mTexCoord.y * -2.f + 1.f) * fViewZ;
	PixelPosition.z = Depth.x * fViewZ; // ����Ÿ���� x�� ������� z�����Ⱑ �� ���̹Ƿ� �ٽ� z���� ���Ѵ�.
	PixelPosition.w = fViewZ;

	PixelPosition = mul(PixelPosition, g_matViewProjInv);

	float4	LightDir = PixelPosition - g_LightPosition;
	// Normalize�ϱ� ���� LightDir�� ���̸� �̸� ���س��´�.
	float	fDistance = length(LightDir);

	// �����ÿ� ������ �޴� �ȼ��� ã�Ƴ��� üũ�ϴ� ������ �ʿ���.
	if (fDistance <= g_fRange)
	{
		float4 Normal = tex2D(NormalSampler, Input.mTexCoord);
		Normal = float4(Normal.xyz * 2.f - 1.f, 0.f);

		// ���ݻ籤 ���ϱ� (����Ʈ Ȯ�����)
		float Diffuse = saturate(dot(normalize(-LightDir), Normal));

		float fAtt = saturate((g_fRange - fDistance) / g_fRange);
		// Ambient + Diffuse
		Output.mDiffuse = (g_LightAmbient * g_MtrlAmbient) + (Diffuse * fAtt) * (g_LightDiffuse * g_MtrlDiffuse);
		Output.mDiffuse *= g_BrightNess;

		float4 fSpecular = 0;
		if (Diffuse > 0)
		{
			// ī�޶� �ΰ��ӻ��� �ȼ��� �ٶ󺸴� ���⺤�͸� ���Ѵ�.
			float3 CamLookDir = normalize(PixelPosition.xyz - g_CamPosition.xyz);
			// �ݻ纤�͸� ���Ѵ�.
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
		// ������� ���⼺ ������ ���� �������� ���ؼ� ���ĺ���.
		AlphaBlendEnable = true;
		SrcBlend = ONE;
		DestBlend = ONE;

		ZEnable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN_DIRECTIONAL();
	}
	pass Point
	{
		// ������� ���⼺ ������ ���� �������� ���ؼ� ���ĺ���.
		AlphaBlendEnable = true;
		SrcBlend = ONE;
		DestBlend = ONE;

		ZEnable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}
}