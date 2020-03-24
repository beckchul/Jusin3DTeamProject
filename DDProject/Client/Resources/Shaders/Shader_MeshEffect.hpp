

float4x4 g_matWorld;
float4x4 g_matWorldViewProj;


float		g_fTime = 0.f;

// 색상
float4		g_vColor = float4(1.f, 1.f, 1.f, 1.f);
// UV
float2		g_vUV = float2(1.f, 1.f);
// UVAnimation
float2		g_vUVAni = float2(0.f, 0.f);
float		g_fUVAniSpeed = 0.f;

texture		g_DiffuseTexture;
sampler BaseSampler = sampler_state
{
	texture	= g_DiffuseTexture;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

texture		g_MaskTexture;
sampler MaskSampler = sampler_state
{
	texture = g_MaskTexture;
//MINFILTER = LINEAR;
//MAGFILTER = LINEAR;
//MIPFILTER = LINEAR;
};


struct VS_IN
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPos = mul(In.vPos, g_matWorldViewProj);

	Out.vTexUV = (In.vTexUV* g_vUV);

	Out.vTexUV.x += g_fTime * g_fUVAniSpeed * g_vUVAni.x;
	Out.vTexUV.y += g_fTime * g_fUVAniSpeed * g_vUVAni.y;

	return Out;
}

struct PS_IN
{
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= g_vColor;
	clamp(Out.vColor, 0.f, 1.f);		// 1 넘지마셈

	return Out;
}

PS_OUT PS_MASK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	//Out.vColor = tex2D(BaseSampler, In.vTexUV);
	//float4 vMaskColor = tex2D(MaskSampler, In.vTexUV);

	//Out.vColor.rgb *= g_vColor.rgb;
	////Out.vColor.a = vMaskColor.r;

	Out.vColor = tex2D(MaskSampler, In.vTexUV);
	//vector vMaskColor = tex2D(BaseSampler, In.vTexUV);

	//Out.vColor *= g_vColor;
	//Out.vColor.rgb *= vMaskColor.rgb;


	return Out;
}

technique Default_Technique
{
	pass AlphaBlend
	{
		LIGHTING = FALSE;
		//CULLMODE = NONE;		 // <- ㅡㅡ
		ALPHABLENDENABLE = TRUE;

		DESTBLEND = ONE/*INVSRCALPHA*/;		//<- 사용했던거 
		SRCBLEND = SRCALPHA;

		//SRCBLEND = SRCALPHA;				// <- 이게 디폴트라고 함
		//DESTBLEND = INVSRCALPHA;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlendCullNone
	{
		LIGHTING = FALSE;
		CULLMODE = NONE;		 // 컬모드 킬건 켜줘야지
		ALPHABLENDENABLE = TRUE;

		DESTBLEND = ONE/*INVSRCALPHA*/;		//<- 사용했던거 
		SRCBLEND = SRCALPHA;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}


	pass AlphaTest
	{
		LIGHTING = FALSE;
		//CULLMODE = NONE;
		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		alpharef = 0x00000008f;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_MaskMap
	{
		LIGHTING = FALSE;
		ALPHABLENDENABLE = TRUE;

		DESTBLEND = ONE;	
		SRCBLEND = SRCALPHA;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MASK();
	}
};
