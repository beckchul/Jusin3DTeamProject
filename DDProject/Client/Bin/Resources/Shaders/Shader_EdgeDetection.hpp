//===================================================================

//===== Constant Table ==============================================

// 외곽선 찾기에 사용할 커널 (소벨 연산자)
// 마스크를 3 X 3 행렬로 선언한다.
float3x3 Kx = { -1, 0, 1,
				-2, 0, 2,
				-1, 0, 1 };

float3x3 Ky = { 1, 2, 1,
				0, 0, 0,
				-1, -2, -1 };

float2 gPixelOffSet;

//===================================================================
//===================================================================


texture g_SrcTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SrcTexture;
};

struct PS_INPUT
{
	float2 mUV : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 mEdge : COLOR0;
};

// 명암의 변화가 없는곳은 검은색 => 즉 기울기값이 거의 없음.
// 명암의 변화가 있는곳은 흰색 => 즉 기울기값이 높다.

// 명암의 변화가 없는곳을 흰색으로..? 1 - L.xxx값.
// 명암의 변화가 있는곳은 검은색으로..? 1 - L.xxx값.

PS_OUTPUT ps_main(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float Lx = 0;
	float Ly = 0;

	// 현재 픽셀을 중앙에 두고 3 X 3 행렬을 - 1에서 1까지 반복한다.
	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			/*x, y에 따라 픽셀의 오프셋을 구한다.
			UV좌표는 0 ~1 범위이다.한 개의 픽셀의 크기는 픽셀의 1 / 픽셀의 폭과  1 / 픽셀의 높이로 구할 수 있다.
			gPixelOffset은 한픽셀의 크기이다.
			텍스쳐로 값을 읽어 온다.*/
			float2 offset = float2(x, y) * gPixelOffSet;
			float3 tex = tex2D(SrcSampler, Input.mUV + offset).rgb;
			//명암의 차이로 윤곽선을 찾는다. 칼라를 흑백으로 변환 시킨다.
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			//소벨 연산자의 마스크 값과 명암을 더한다.
			Lx += luminance * Kx[y+1][x+1];
			Ly += luminance * Ky[y+1][x+1];
		}
	}

	//기울기를 구한다.
	float L = sqrt((Lx * Lx) + (Ly * Ly));

	//명암의 변화가 없는곳은 검은색, 있는곳은 하얀색을 반환한다.
	//칼라값이 동일해야 흑백으로 나오기 때문에 xxx를 반환한다.
	Output.mEdge = float4(1- L.xxx, 1);

	return Output;
}


technique Default_Device
{
	pass Default_Rendering
	{
		lighting = false;
		Zenable = false;
		Zwriteenable = false;

		PixelShader = compile ps_3_0 ps_main();
	}
}