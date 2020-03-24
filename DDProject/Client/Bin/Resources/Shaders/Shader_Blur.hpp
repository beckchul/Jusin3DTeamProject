
texture m_TxDif;
sampler smpDif = sampler_state
{
	texture = <m_TxDif>;
	AddressU = Clamp;
	AddressV = Clamp;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct SvsOut
{
	float4	Pos : POSITION;
	float2	Tex : TEXCOORD0;
};

float	m_TexW /*= 800*/;
float	m_TexH /*= 600*/;


static const float Weight[13]=
{
   0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561};

static const float Total = 6.2108;

SvsOut VtxProc0(float3 Pos : POSITION
	, float2 Tex : TEXCOORD0)
{
	SvsOut Out = (SvsOut)0;

	Out.Pos = float4(Pos, 1);
	Out.Tex = Tex;

	return Out;
}


float4 PxlBlurX(SvsOut In) : COLOR0
{
	float4	Out=0;

	float2	t = In.Tex;
	float2	uv = 0;

	float	tu= 1./m_TexW;

	for(int i=-6; i<6; ++i)
	{
		uv = t+ float2(tu *i, 0);
		Out += Weight[6+i] * tex2D(smpDif, uv);
	}

	Out /=Total;

	return Out;
}


float4 PxlBlurY(SvsOut In) : COLOR0
{
	float4	Out=0;

	float2	t = In.Tex;
	float2	uv = 0;

	float	tv= 1./m_TexH;

	for(int i=-6; i<6; ++i)
	{
		uv = t+ float2(0, tv *i);
		Out += Weight[6+i] * tex2D(smpDif, uv);
	}

	Out /=Total;

	return Out;
}

//// texture
//texture m_TxDif;
//sampler smpDif = sampler_state
//{
//	texture = <m_TxDif>;
//	AddressU = Clamp;
//	AddressV = Clamp;
//
//	MinFilter = LINEAR;
//	MagFilter = LINEAR;
//	MipFilter = LINEAR;
//};

//struct SvsOut
//{
//	float4	Pos : POSITION;
//	float2	Tex : TEXCOORD0;
//};
//
//float	m_TexW /*= 800*/;
//float	m_TexH /*= 600*/;
//
//// for Blur
//static float fBgn = 7;
//static float fIns=.1f;
//static float fInc=1.f;
//static float fDelta= 0.05f;
//
//// Blur X
//float4 PxlBlurX(SvsOut In) : COLOR0
//{
//	float4	Out=0;
//	float2	uv = 0;
//
//	for(float i=-fBgn; i<=fBgn; i+=1)
//	{
//		uv = In.Tex + float2((i*fInc+1.5)/m_TexW, 0);
//		Out += tex2D(smpDif, uv) * exp( -i*i * fDelta);
//	}
//
//	Out *=fIns;
//	Out.w = 1;
//	return Out;
//}
//
//
//// Blur Y
//float4 PxlBlurY(SvsOut In) : COLOR0
//{
//	float4	Out=0;
//	float2	uv = 0;
//
//	for(float i=-fBgn; i<=fBgn; i+=1)
//	{
//		uv = In.Tex + float2(0, (i*fInc+1.5)/m_TexH);
//		Out += tex2D(smpDif, uv) * exp( -i*i * fDelta);
//	}
//
//	Out *=fIns;
//	Out.w = 1;
//	return Out;
//}

technique Tech
{
	pass P0
	{
		lighting = false;
	
		Zenable = false;
		Zwriteenable = false;

		VertexShader = compile vs_1_1 VtxProc0();
		PixelShader  = compile ps_3_0 PxlBlurX();
	}

	pass P1
	{
		lighting = false;

		Zenable = false;
		Zwriteenable = false;

		VertexShader = compile vs_1_1 VtxProc0();
		PixelShader  = compile ps_3_0 PxlBlurY();
	}
};