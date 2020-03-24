//===================================================================

//===== Constant Table ==============================================

float4x4 g_matViewInv;
float4x4 g_matProjInv;
float4x4 g_matLightView, g_matLightProj;


//===================================================================
//===================================================================

texture			g_ColorTexture;

sampler ColorSampler = sampler_state
{
	texture = g_ColorTexture;
};


texture			g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
};

texture			g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
};


texture			g_EdgeNormalTexture;

sampler EdgeNormalSampler = sampler_state
{
	texture = g_EdgeNormalTexture;
};

texture			g_EdgeDepthTexture;

sampler EdgeDepthSampler = sampler_state
{
	texture = g_EdgeDepthTexture;
};


texture			g_GlowSourcesTexture;

sampler GlowSampler = sampler_state
{
	texture = g_GlowSourcesTexture;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};


struct PS_IN
{
	float2 mUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 mColor : COLOR0; 
};


PS_OUT PS_MAIN(PS_IN Input)
{
	PS_OUT Output = (PS_OUT)0.f;

	float4 color	= tex2D(ColorSampler, Input.mUV);
	float4 diffuse	= tex2D(ShadeSampler, Input.mUV);
	float4 specular = tex2D(SpecularSampler, Input.mUV);

	float4 edgeNormal	= tex2D(EdgeNormalSampler, Input.mUV);
	float4 edgeDepth	= tex2D(EdgeDepthSampler, Input.mUV);

	float4 glow = tex2D(GlowSampler, Input.mUV);

	// glow로 빛나는 부분은 조명연산에서 제외한다.	
	float4 ResultColor = ( color * diffuse * edgeNormal * edgeDepth) + specular;

	Output.mColor = glow + ResultColor;

	return Output;
}

PS_OUT PS_BADCOM(PS_IN Input)
{
	PS_OUT Output = (PS_OUT)0.f;

	float4 color = tex2D(ColorSampler, Input.mUV);
	float4 diffuse = tex2D(ShadeSampler, Input.mUV);
	float4 specular = tex2D(SpecularSampler, Input.mUV);

	Output.mColor = color * diffuse + specular;

	return Output;
}

technique Default_Device
{
	pass Blend
	{
		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass BadComBlend
	{
		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 PS_BADCOM();
	}
}