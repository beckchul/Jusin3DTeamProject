//===================================================================

//===== Constant Table ==============================================

// �ܰ��� ã�⿡ ����� Ŀ�� (�Һ� ������)
// ����ũ�� 3 X 3 ��ķ� �����Ѵ�.
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

// ����� ��ȭ�� ���°��� ������ => �� ���Ⱚ�� ���� ����.
// ����� ��ȭ�� �ִ°��� ��� => �� ���Ⱚ�� ����.

// ����� ��ȭ�� ���°��� �������..? 1 - L.xxx��.
// ����� ��ȭ�� �ִ°��� ����������..? 1 - L.xxx��.

PS_OUTPUT ps_main(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float Lx = 0;
	float Ly = 0;

	// ���� �ȼ��� �߾ӿ� �ΰ� 3 X 3 ����� - 1���� 1���� �ݺ��Ѵ�.
	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			/*x, y�� ���� �ȼ��� �������� ���Ѵ�.
			UV��ǥ�� 0 ~1 �����̴�.�� ���� �ȼ��� ũ��� �ȼ��� 1 / �ȼ��� ����  1 / �ȼ��� ���̷� ���� �� �ִ�.
			gPixelOffset�� ���ȼ��� ũ���̴�.
			�ؽ��ķ� ���� �о� �´�.*/
			float2 offset = float2(x, y) * gPixelOffSet;
			float3 tex = tex2D(SrcSampler, Input.mUV + offset).rgb;
			//����� ���̷� �������� ã�´�. Į�� ������� ��ȯ ��Ų��.
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			//�Һ� �������� ����ũ ���� ����� ���Ѵ�.
			Lx += luminance * Kx[y+1][x+1];
			Ly += luminance * Ky[y+1][x+1];
		}
	}

	//���⸦ ���Ѵ�.
	float L = sqrt((Lx * Lx) + (Ly * Ly));

	//����� ��ȭ�� ���°��� ������, �ִ°��� �Ͼ���� ��ȯ�Ѵ�.
	//Į���� �����ؾ� ������� ������ ������ xxx�� ��ȯ�Ѵ�.
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