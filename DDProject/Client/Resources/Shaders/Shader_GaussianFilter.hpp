texture g_SampleTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SampleTexture;
};

float4 gaussFilter_Y[7] =
{
	0.0, -3.0, 0.0, 1.0 / 64.0,
	0.0, -2.0, 0.0, 6.0 / 64.0,
	0.0, -1.0, 0.0, 15.0 / 64.0,
	0.0, 0.0, 0.0, 20.0 / 64.0,
	0.0, 1.0, 0.0, 15.0 / 64.0,
	0.0, 2.0, 0.0, 6.0 / 64.0,
	0.0, 3.0, 0.0, 1.0 / 64.0
};

float4 gaussFilter_X[7] =
{
	-3.0, 0.0, 0.0, 1.0 / 64.0,
	-2.0, 0.0, 0.0, 6.0 / 64.0,
	-1.0, 0.0, 0.0, 15.0 / 64.0,
	0.0, 0.0, 0.0, 20.0 / 64.0,
	1.0, 0.0, 0.0, 15.0 / 64.0,
	2.0, 0.0, 0.0, 6.0 / 64.0,
	3.0, 0.0, 0.0, 1.0 / 64.0
};

float2 texScaler;

float texOffset = 0.0;

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};


PS_OUTPUT PS_BLUR_X_MAIN(PS_INPUT In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	int i;
	float4 color = 0.0;
	for (i = 0; i < 7; ++i)
	{
		color += tex2D(SrcSampler
			, float2(In.TexCoord.x + gaussFilter_X[i].x * texScaler.x + texOffset
				, In.TexCoord.y + gaussFilter_X[i].y * texScaler.y + texOffset)) * gaussFilter_X[i].w;
	} //End For

	Out.Color = color;

	return Out;
}

PS_OUTPUT PS_BLUR_Y_MAIN(PS_INPUT In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;

	int i;
	float4 color = 0.0;
	for (i = 0; i < 7; ++i)
	{
		color += tex2D(SrcSampler
			, float2(In.TexCoord.x + gaussFilter_Y[i].x * texScaler.x + texOffset
				, In.TexCoord.y + gaussFilter_Y[i].y * texScaler.y + texOffset)) * gaussFilter_Y[i].w;
	} //End For

	Out.Color = color;

	return Out;
}

technique Default_Device
{
	pass Blur_X
	{
		lighting = false;
		Zenable = false;
		Zwriteenable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR_X_MAIN();
	}

	pass Blur_Y
	{
		lighting = false;
		Zenable = false;
		Zwriteenable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR_Y_MAIN();
	}
}