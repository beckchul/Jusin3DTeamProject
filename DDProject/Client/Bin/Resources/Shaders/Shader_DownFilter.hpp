//===================================================================

//===== Constant Table ==============================================




//===================================================================
//===================================================================



//===================================================================
// 1/4축소버퍼에 장면을 복사한다.
// (부동소수점 포맷에서는 포인트 샘플링밖에 할 수 없다.)
//===================================================================


float2 g_avSampleOffsets[16];

texture g_SrcTexture;

sampler s0 = sampler_state
{
	texture = g_SrcTexture;

	MinFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct PS_INPUT
{
	float2 mUV : TEXCOORD0;
};


float4 PS_MAIN(PS_INPUT Input) : COLOR0
{
	float4 sample = 0.0f;

	for( int i=0; i < 16; i++ ) {
		sample += tex2D( s0, Input.mUV + g_avSampleOffsets[i] );
	}
    
	return sample / 16;
}

technique Default_Device
{
	pass Rendering
	{
		Zenable = false;
		Zwriteenable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}