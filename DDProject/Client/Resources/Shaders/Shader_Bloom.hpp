float map_cx;                        //텍스처 크기  가로 
float map_cy;                        //텍스처 크기  세로


texture g_SrcTexture;

sampler Sampler0 = sampler_state
{
	texture = g_SrcTexture;
};


texture g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};


float2 rcpres = { 0.0025, 0.0033333333333333333333333333333333 };

//텍스처 의 임의의 위치의 픽셀에 접근하귀 위한 좌표 ( 가로 방향)
float2 PixelKernelH[13] =
{
	{ -6, 0 },{ -5, 0 },{ -4, 0 },{ -3, 0 },{ -2, 0 },{ -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },
	{ 5, 0 },{ 6, 0 },
};

//텍스처의 임의의 위치의 픽셀에 접근하기 위한 좌표 ( 세로 방향)
float2 PixelKernelV[13] =
{
	{ 0, -6 },{ 0, -5 },{ 0, -4 },{ 0, -3 },{ 0, -2 },{ 0, -1 },{ 0,  0 },{ 0,  1 },{ 0,  2 },{ 0,  3 },{ 0,  4 },
	{  0,  5 },{ 0,  6 },
};

//미리 계산해 둔 가우스 필터의 마스크 값
float BlurWeights[13] =
{
	0.002216,    0.008764,    0.026995,    0.064759,    0.120985,    0.176033,    0.199471,    0.176033,    0.120985,    0.064759,
	0.026995,    0.008764,    0.002216,
};


float4 PSBlur(float2 Tex : TEXCOORD) : COLOR0
{
	float4 Depth = tex2D(DepthSampler, Tex);

	// Dpeth값 기록안한 부분의 픽셀은 블룸처리 거르자.. (Glow라서 심하게 밝아짐)
	if (Depth.b >= 1.f)
		return float4(0.f, 0.f, 0.f, 0.f);

	float4 Color = tex2D(Sampler0, Tex);

	Color = pow(Color, 4);

	float4 Color2 = -0.84;

	for (int index = 0; index < 13; ++index) 
	{
		Color2 += tex2D(Sampler0, Tex + (PixelKernelH[index] * rcpres)) * BlurWeights[index];
		Color2 += tex2D(Sampler0, Tex + (PixelKernelV[index] * rcpres)) * BlurWeights[index];
	}

	Color2 *= 0.48;

	float4 Color3 = Color + Color2;
	//Color3.a = 0.f;

	// Color3을 기존화면과 합치면 됨.
	return Color3;
}


technique GOneBloom 
{
	pass P0
	{
		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 PSBlur();
	}
}