//===================================================================

//===== Constant Table ==============================================

float3 g_BackColor = float3(0.f, 0.f, 0.f);
float3 g_FadeColor = float3(0.f, 0.f, 0.f);


//===================================================================
//===================================================================

texture g_SceneTexture;

sampler SceneSampler = sampler_state
{
	texture = g_SceneTexture;
};

texture g_BloomTexture;

sampler BloomSampler = sampler_state
{
	texture = g_BloomTexture;
	
	AddressU = Clamp;
	AddressV = Clamp;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

texture g_BlurTexture;

sampler BlurSampler = sampler_state
{
	texture = g_BlurTexture;

	AddressU = Clamp;
	AddressV = Clamp;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};


texture g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};



struct PS_INPUT
{
	float2 mUV : TEXCOORD0;
};


float4 PS_MAIN(PS_INPUT Input) : COLOR0
{
	float4 SceneColor = tex2D(SceneSampler, Input.mUV);
	float4 BlurColor = tex2D(BlurSampler, Input.mUV);

	float Depth = tex2D(DepthSampler, Input.mUV).b;

	//float4 FinalColor = (SceneColor + BlurColor) / 2;
	float4 FinalColor;

	
	if (Depth >= 1.f)
	{
		FinalColor = (SceneColor + BlurColor) / 2;
	}
	else
		FinalColor = SceneColor * 0.9 + BlurColor * 0.5;

	FinalColor.rgb *= g_FadeColor;

	FinalColor.rgb += g_BackColor;
	
	return FinalColor;
}

technique Default_Device
{
	pass Rendering
	{
		Zenable = false;
		Zwriteenable = false;

		// ����ۿ� ���ĺ��� �ռ��Ѵ�.(�ΰ� ��ī�̹ڽ��� �������� ���ؼ�)
		AlphaBlendEnable = true;

		//SrcBlend = SrcAlpha;
		//DestBlend = InvSrcAlpha;

		//DEST����*DEST�̹��� + SRC����*SRC�̹���>�� �����ȴ�.(ADD�ռ���)
		SrcBlend = One;
		DestBlend = InvSrcAlpha;

		PixelShader = compile ps_3_0 PS_MAIN();
	}
}