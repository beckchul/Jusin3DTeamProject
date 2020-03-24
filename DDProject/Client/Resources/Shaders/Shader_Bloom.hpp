float map_cx;                        //�ؽ�ó ũ��  ���� 
float map_cy;                        //�ؽ�ó ũ��  ����


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

//�ؽ�ó �� ������ ��ġ�� �ȼ��� �����ϱ� ���� ��ǥ ( ���� ����)
float2 PixelKernelH[13] =
{
	{ -6, 0 },{ -5, 0 },{ -4, 0 },{ -3, 0 },{ -2, 0 },{ -1, 0 },{ 0, 0 },{ 1, 0 },{ 2, 0 },{ 3, 0 },{ 4, 0 },
	{ 5, 0 },{ 6, 0 },
};

//�ؽ�ó�� ������ ��ġ�� �ȼ��� �����ϱ� ���� ��ǥ ( ���� ����)
float2 PixelKernelV[13] =
{
	{ 0, -6 },{ 0, -5 },{ 0, -4 },{ 0, -3 },{ 0, -2 },{ 0, -1 },{ 0,  0 },{ 0,  1 },{ 0,  2 },{ 0,  3 },{ 0,  4 },
	{  0,  5 },{ 0,  6 },
};

//�̸� ����� �� ���콺 ������ ����ũ ��
float BlurWeights[13] =
{
	0.002216,    0.008764,    0.026995,    0.064759,    0.120985,    0.176033,    0.199471,    0.176033,    0.120985,    0.064759,
	0.026995,    0.008764,    0.002216,
};


float4 PSBlur(float2 Tex : TEXCOORD) : COLOR0
{
	float4 Depth = tex2D(DepthSampler, Tex);

	// Dpeth�� ��Ͼ��� �κ��� �ȼ��� ���ó�� �Ÿ���.. (Glow�� ���ϰ� �����)
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

	// Color3�� ����ȭ��� ��ġ�� ��.
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