float4x4 g_matWorld;
float4x4 g_matLightView; // ±¤¿ø - ºä
float4x4 g_matLightProj; // ±¤¿ø - Åõ¿µ


float4x4 MatrixPalette[53];
int numBoneInfluences = 2;



	
struct VS_INPUT
{
	float4 mPosition : POSITION;
};

struct VS_OUTPUT
{
	float4 mPosition : POSITION;
	float4 mClipPosition : TEXCOORD1;
};

VS_OUTPUT VS_MAIN(VS_INPUT Input)
{
	VS_OUTPUT Output;

	Output.mPosition = mul(Input.mPosition, g_matWorld);
	Output.mPosition = mul(Output.mPosition, g_matLightView);
	Output.mPosition = mul(Output.mPosition, g_matLightProj);

	Output.mClipPosition = Output.mPosition;

	return Output;
}


struct VS_DYNAMIC_IN
{
	float4	mPosition : POSITION;
	float4	weights : BLENDWEIGHT0;
	int4	boneIndices : BLENDINDICES0;
};

struct VS_DYNAMIC_OUT
{
	float4 mPosition : POSITION;
	float4 mClipPosition : TEXCOORD1;
};


VS_DYNAMIC_OUT VS_DYNAMIC_MAIN(VS_DYNAMIC_IN In)
{
	VS_DYNAMIC_OUT	Out = (VS_DYNAMIC_OUT)0;

	//float4 worldPos = mul(In.mPosition, g_matWorld);

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	//float3 norm = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = numBoneInfluences - 1;

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.mPosition, MatrixPalette[In.boneIndices[i]]);

		//norm += In.weights[i] * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[i]]);
	}

	lastWeight = 1.0f - lastWeight;
	p += lastWeight * mul(In.mPosition, MatrixPalette[In.boneIndices[n]]);

	//norm += lastWeight * mul(In.mNormal, (float3x3)MatrixPalette[In.boneIndices[n]]);
	p.w = 1.0f;

	Out.mPosition = mul(p, g_matWorld);
	Out.mPosition = mul(Out.mPosition, g_matLightView);
	Out.mPosition = mul(Out.mPosition, g_matLightProj);

	Out.mClipPosition = Out.mPosition;

	return Out;
}




struct PS_INPUT
{
	float4 mClipPosition : TEXCOORD1;
};

float4 PS_MAIN(PS_INPUT Input) : COLOR
{
	float depth = Input.mClipPosition.z / Input.mClipPosition.w;
	return float4(depth.xxx, 1.f);
}

technique CreateShadowShader
{
	pass CreateShadowStatic
	{
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass CreateShadowStataic
	{
		lighting = false;

		VertexShader = compile vs_3_0 VS_DYNAMIC_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}