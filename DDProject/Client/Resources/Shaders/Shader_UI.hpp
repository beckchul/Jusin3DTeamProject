
float4x4		g_matWorldViewProj;

texture			g_BaseTexture, g_MaskTexture, g_ColorTexture, g_ColorTexture2;

vector			g_fColor = { 1.f, 1.f, 1.f, 1.f };
float			g_fAlpha = 1.f;
float			g_fTime = 0.f;
float			g_fRatio = 0.f;

sampler BaseSampler = sampler_state
{
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	texture = g_BaseTexture;
};

// 마스크용
sampler MaskSampler = sampler_state
{
	texture = g_MaskTexture;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

// 색깔 입히기용
sampler ColorSampler = sampler_state
{
	texture = g_ColorTexture;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

sampler ColorSampler2 = sampler_state
{
	texture = g_ColorTexture2;
	MINFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

struct VS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_MASK_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		vTexChangeUV : TEXCOORD1;
	float2		vTexMaskUV : TEXCOORD2;
};

struct VS_MERGE_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		vTexChangeUV : TEXCOORD1;
	float2		vTexMaskUV : TEXCOORD2;
};

struct VS_BaseColor_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float2		vTexChangeUV : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_MERGE_OUT VS_MASK(VS_IN In)
{
	VS_MERGE_OUT		Out = (VS_MERGE_OUT)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);

	Out.vTexUV = In.vTexUV;
	Out.vTexChangeUV = (In.vTexUV) + float2(g_fTime * 0.5f, g_fTime * 0.5f);

	Out.vTexMaskUV = In.vTexUV;

	return Out;
}

VS_MERGE_OUT VS_Merge(VS_IN In)
{
	VS_MERGE_OUT		Out = (VS_MERGE_OUT)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);
	
	Out.vTexUV = In.vTexUV;
	Out.vTexChangeUV = (In.vTexUV) + float2(g_fTime * 0.5f, 0.f);

	Out.vTexMaskUV = In.vTexUV + float2(g_fTime * 0.5f, 0.f);
	Out.vTexMaskUV.y = Out.vTexMaskUV.y * 0.5f - 0.5f - g_fRatio;

	return Out;
}

VS_BaseColor_OUT VS_BaseColor(VS_IN In)
{
	VS_BaseColor_OUT Out = (VS_BaseColor_OUT)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);
	Out.vTexUV = In.vTexUV;
	
	Out.vTexChangeUV = (In.vTexUV) + float2(g_fTime * 0.5f, 0.f);
	
	return Out;
}

// Pixel Shader
struct PS_IN
{
	float2				vTexUV : TEXCOORD0;
};

struct PS_MERGE_IN
{
	float2				vTexUV : TEXCOORD0;
	float2				vTexChangeUV : TEXCOORD1;
	float2				vTexMaskUV : TEXCOORD2;
};

struct PS_BaseColor_In
{
	float2				vTexUV : TEXCOORD0;
	float2				vTexChangeUV : TEXCOORD1;
};

struct PS_OUT
{
	vector				vColor : COLOR0;
};

// AlphaTest
PS_OUT PS_AlphaTest(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_DarkRemoveAlphaTest(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;

	return Out;
}

// AlphaBlend
PS_OUT PS_AlphaBlend(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

// Alpha 곱하기
PS_OUT PS_AlphaMultiply(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a *= g_fAlpha;

	return Out;
}

// AlphaTest + AlphaBlend
PS_OUT PS_AlphaTestBlend(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	if(Out.vColor.a > 0.1f)
		Out.vColor.a = 0.8f; // 반투명한 미니맵

	return Out;
}

// SkillIcon (색깔 곱하기)
PS_OUT PS_SkillIcon(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= g_fColor;

	return Out;
}

// Tower Radius
PS_OUT PS_TowerRadius(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	
	Out.vColor *= g_fColor;

	if (Out.vColor.a > 0.2f)
		Out.vColor.a = (pow((In.vTexUV.x - 0.5f), 2.f) + pow((In.vTexUV.y - 0.5f), 2.f)) * (1 - g_fAlpha) / 0.125f + g_fAlpha;
	
	return Out;
}

// Mask
PS_OUT PS_Mask(PS_MERGE_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	vector vColor = tex2D(BaseSampler, In.vTexUV); // Panel
	vector vMaskColor = tex2D(MaskSampler, In.vTexMaskUV);
	vector vTextureColor = tex2D(ColorSampler, In.vTexChangeUV);

	Out.vColor = vColor;

	if (In.vTexUV.x < g_fRatio)
		Out.vColor += (vTextureColor * vector(2.f, 2.f, 2.f, 1.f) + g_fColor) * vMaskColor;

	Out.vColor.a *= Out.vColor.x; // 검은 부분 알파테스팅

	return Out;
}

// Merge
PS_OUT PS_Merge(PS_MERGE_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	vector vMaskColor = tex2D(MaskSampler, In.vTexMaskUV);
	vector vTextureColor = tex2D(ColorSampler, In.vTexChangeUV);
	vector vTextureColor2 = tex2D(ColorSampler2, In.vTexChangeUV);

	Out.vColor = (vTextureColor + vTextureColor) * vector(0.1f, 0.1f, 0.1f, 1.f) + g_fColor;

	Out.vColor *= vMaskColor;
	Out.vColor.a = vMaskColor.x; // 마스크맵이 검은색인 곳은 알파블렌딩 처리

	return Out;
}

// Base + Color
PS_OUT PS_BaseColor(PS_BaseColor_In In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	vector vBaseColor = tex2D(BaseSampler, In.vTexUV);
	vector vTextureColor = tex2D(ColorSampler, In.vTexChangeUV);

	if (In.vTexUV.x < g_fRatio)
	{
		Out.vColor = (vBaseColor * vTextureColor) * g_fColor;
		Out.vColor.a = vBaseColor.x; // 검은 영역 알파블렌드 처리
	}

	return Out;
}

technique Default_Device
{
	pass AlphaTest
	{	
		ALPHABLENDENABLE = FALSE;

		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		alpharef = 0x00000008f;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_AlphaTest();
	}

	pass AlphaBlend
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_AlphaBlend();
	}

	pass DarkRemove		 
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		BLENDOP = ADD;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		LIGHTING = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_AlphaBlend();
	}

	pass DarkRemoveAlphaTest
	{
		ALPHABLENDENABLE = FALSE;

		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		alpharef = 0x00000001f;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DarkRemoveAlphaTest();

	}

	pass DarkRemoveColorMultiply
	{

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		BLENDOP = ADD;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		LIGHTING = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SkillIcon();
	}

	pass AlphaMultiply
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_AlphaMultiply();
	}

	pass AlphaTestBlend
	{
		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		alpharef = 0x00000004f;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_AlphaTestBlend();
	}

	pass ColorMultiply
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SkillIcon();
	}

	pass TowerRaidus
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		ZWRITEENABLE = FALSE;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_TowerRadius();
	}

	pass UI_Mask
	{
		ALPHABLENDENABLE = FALSE;

		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		alpharef = 0x00000001f;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_MASK();
		PixelShader = compile ps_3_0 PS_Mask();
	}

	pass UI_Merge // Hp, Mp
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;
		
		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_Merge();
		PixelShader = compile ps_3_0 PS_Merge();
	}

	pass UI_BaseColor
	{
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = INVSRCALPHA;
		SRCBLEND = SRCALPHA;

		CULLMODE = none;
		lighting = false;

		VertexShader = compile vs_3_0 VS_BaseColor();
		PixelShader = compile ps_3_0 PS_BaseColor();

	}
}