
texture g_SampleTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SampleTexture;
};
 
texture g_SampleTexture2; // 다운샘플링 된 녀석이라 LINEAR로
sampler SrcSampler2 = sampler_state
{
	texture = g_SampleTexture2;
	
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

struct PS_OUTPUT
{
	float4 mColor : COLOR0;
};

// 기존 Color의 rgb값과 a값에 넣어놓은 값을 곱한다.
PS_OUTPUT PS_MAIN(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.f;

	float4 color = tex2D(SrcSampler, Input.mUV);

	color = float4(color.rgb * color.a, 0.f);

	Output.mColor = color;

	return Output;
}


// 기존 Color의 rgb와 Glow를 더한다.
PS_OUTPUT PS_MAIN2(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.f;

	float4 glowSource = tex2D(SrcSampler2, Input.mUV); // 이건 Blur처리가 된거.


	//깊이로 따져서 깊이가 0인부분 => 즉 조명연산이 없는 부분에 대해서만 
	//Depth에서 깊이기록이 안된부분에 대해서만 처리해보자.
	float4 color = (float4)0;
	float depth = tex2D(DepthSampler, Input.mUV).r;
	
	if (depth == 1)
		 color = tex2D(SrcSampler, Input.mUV);
	
	Output.mColor = float4( color.rgb + glowSource.rgb, 0.f);

	return Output;
}

technique Default_Device
{
	pass pass1
	{
		lighting = false;

		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass pass2
	{
		lighting = false;

		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 PS_MAIN2();
	}
}