//===================================================================

//===== Constant Table ==============================================




//===================================================================
//===================================================================

texture g_SrcTexture;

sampler s0 = sampler_state
{
	texture = g_SrcTexture;
	AddressU = BORDER;
	AddressV = BORDER;
};

struct PS_INPUT
{
	float2 mUV : TEXCOORD0;
};


float4 BrightPassFilter(PS_INPUT Input) : COLOR
{
	float4 vSample = tex2D( s0, Input.mUV );
	
	// ȭ�鿡�� ���� �κ�(����� ����� �κ�)�� ã�´�?

	// ��ο� �κ� ����
	vSample.rgb -= 0.5f;
	
	// ���Ѱ��� 0����
	vSample = 3.0f*max(vSample, 0.0f);

	return vSample;
}

technique Default_Device
{
	pass Rendering
	{
		Zenable = false;
		Zwriteenable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 BrightPassFilter();
	}
}