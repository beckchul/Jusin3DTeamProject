 
float4x4		g_matWorld;
float4x4		g_matWorldViewProj;

texture			g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	texture = g_DiffuseTexture;
};

texture			g_CloudMaskTexture;

sampler MaskSampler = sampler_state
{
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	texture = g_CloudMaskTexture;
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
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);

	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float3				vTexUV : TEXCOORD0;
};


struct PS_OUTPUT
{
	float4 mColor : COLOR0;
};

PS_OUTPUT PS_SKYBOX_MAIN(PS_IN In)
{
	PS_OUTPUT		Output = (PS_OUTPUT)0;

	Output.mColor = tex2D(DiffuseSampler, In.vTexUV);
	Output.mColor.a = 0.7;

	return Output;
}


PS_OUTPUT PS_SKYBOXBALL_MAIN(PS_IN In)
{
	PS_OUTPUT		Output = (PS_OUTPUT)0;

	float4 mask = tex2D(MaskSampler, In.vTexUV);

	float4 albedo = tex2D(DiffuseSampler, In.vTexUV);
	albedo.a = 0.3;


	albedo.rgb = albedo.rgb * mask.rgb * float3(0, 0.3294, 1);

	Output.mColor = albedo;

	return Output;
}


technique Default_Device
{
	pass SkyBoxRendering	
	{	
		lighting = false;
		//Cullmode = none;

		AlphaBlendEnable = true;

		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		//Zenable = false;
		//Zwriteenable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SKYBOX_MAIN();
	}

	pass SkyBoxBallRendering
	{	
		lighting = false;
		//Cullmode = none;

		AlphaBlendEnable = true;

		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		//Zenable = false;
		//Zwriteenable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SKYBOXBALL_MAIN();
	}
}