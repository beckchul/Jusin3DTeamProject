

float4x4 g_matWorld;
float4x4 g_matWorldInv;

float4x4 g_matWorldView;
float4x4 g_matWorldViewProj;

float4x4 g_matProjInv;
float4x4 g_matViewInv;
float4x4 g_matViewProjInv;

float2 Deproject;
float4 CameraRightTop;

// ---------------------------------------------------------------
// 색상
float4		g_vColor = float4(1.f, 1.f, 1.f, 1.f);
// 시간
float		g_fTime = 0.f;
// UV
float2		g_vUV = float2(1.f, 1.f);
// UVAnimation
float2		g_vUVAni = float2(0.f, 0.f);
float		g_fUVAniSpeed = 0.f;


float fFadeSpeed = 0.f;	// 연기 이펙트 해당. . . 페이드되는 속도.

// Decal Op
float fDecalMaskFadeIn = 0.f;	// 데칼 이펙트의 마스크 이미지의 페이드 인 정도
float fDecalFadeOut = 0.f;			// 데칼 이펙트 페이드 아웃. -> 페이드 인이 완전히 되면 되게.
float fBlendTime = 10.f;
float3 vMaskColor = float3(1.f, 1.f, 1.f);
bool bMaskOn = true;

float	m_TexW = 256;
float	m_TexH = 256;

static const float Weight[13] =
{
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561 };

static const float Total = 6.2108;

float4 g_worldCameraPosition;

float3 g_LightColor = float3(1.f, 1.f, 1.f);



texture			g_DiffuseTexture;
sampler BaseSampler = sampler_state
{
	mipfilter = linear;
	minfilter = linear;
	magfilter = linear;
	texture = g_DiffuseTexture;
};

texture		g_MaskTexture;
sampler MaskSampler = sampler_state
{
	mipfilter = linear;
	minfilter = linear;
	magfilter = linear;
	texture = g_MaskTexture;
};

texture		g_FadeTexture;
sampler FadeSampler = sampler_state
{
	texture = g_FadeTexture;
};
//
//texture		g_SamplerNormalTexture;
//sampler NormalSampler = sampler_state
//{
//	texture = g_SamplerNormalTexture;
//
//	MAGFILTER = LINEAR;
//	MINFILTER = LINEAR;
//};

texture			g_DepthTexture;
sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

texture			g_NormalTexture;
sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};
// ---------------------------------------------------------------


// Vertex_Shader---------------------------------------------
struct VS_IN_2D
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_2D
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
	float3		vOrientationX : TEXCOORD2;
	float3		vOrientationY : TEXCOORD3;
	float3		vOrientationZ : TEXCOORD4;
};


struct VS_IN_MESH
{
	vector		vPos : POSITION;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT_MESH
{
	vector		vPos : POSITION;
	float2			vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
};

struct VS_FORWARD_INPUT
{
	float4	mPosition : POSITION;
	float3  mNormal : NORMAL;
	float3	mTangent : TANGENT;
	float3  mBinormal : BINORMAL;
	float2	mUV : TEXCOORD0;
};

struct VS_FORWARD_OUTPUT
{
	float4 mPosition : POSITION;
	float2 mUV : TEXCOORD0;
	float3 mLightDir : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};

VS_OUT_2D VS_2DEFFECT(VS_IN_2D In)
{
	VS_OUT_2D		Out = (VS_OUT_2D)0.0f;

	Out.vPosition = mul(In.vPosition, g_matWorldViewProj);

	Out.vTexUV = In.vTexUV;

	/*Out.vTexUV = (In.vTexUV * g_vUV);
	Out.vTexUV.x += g_fTime * g_fUVAniSpeed * g_vUVAni.x;
	Out.vTexUV.y += g_fTime * g_fUVAniSpeed * g_vUVAni.y;*/

	Out.vProjPos = Out.vPosition;

	// zy일때는 x축 방위방향으로
	Out.vOrientationX = normalize(g_matWorldView[0].xyz);
	// xz일때는 y축 방위방향으로
	Out.vOrientationY = normalize(g_matWorldView[1].xyz);
	// xy일때는 z축 방위방향으로
	Out.vOrientationZ = normalize(g_matWorldView[2].xyz);

	return Out;
}

VS_OUT_MESH VS_MESHEFFECT(VS_IN_MESH In)
{
	VS_OUT_MESH		Out = (VS_OUT_MESH)0;

	Out.vPos = mul(In.vPos, g_matWorldViewProj);

	Out.vTexUV = (In.vTexUV/** g_vUV*/);

	Out.vTexUV.x += g_fTime * g_fUVAniSpeed * g_vUVAni.x;
	Out.vTexUV.y += g_fTime * g_fUVAniSpeed * g_vUVAni.y;

	Out.vProjPos = Out.vPos;

	return Out;
}



VS_FORWARD_OUTPUT VS_BUMP_MAIN(VS_FORWARD_INPUT Input)
{
	VS_FORWARD_OUTPUT Output = (VS_FORWARD_OUTPUT)0;

	Output.mPosition = mul(Input.mPosition, g_matWorldViewProj);

	float4 worldPos = mul(Input.mPosition, g_matWorld);

	Output.N = normalize(mul(Input.mNormal, (float3x3)g_matWorld));
	Output.T = normalize(mul(Input.mTangent, (float3x3)g_matWorld));
	Output.B = normalize(mul(Input.mBinormal, (float3x3)g_matWorld));

	Output.mUV = Input.mUV;

	Output.mUV.x += g_fTime * g_fUVAniSpeed * g_vUVAni.x;
	Output.mUV.y += g_fTime * g_fUVAniSpeed * g_vUVAni.y;

	Output.mLightDir = normalize(float3(0.f, -0.57735f, 0.f));

	float3 viewDir = normalize(worldPos.xyz - g_worldCameraPosition.xyz);
	Output.mViewDir = viewDir;

	return Output;
}

// -------------------------------------------------------------

// Pixel Shader

struct PS_IN_2D
{
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
	float3		vOrientationX : TEXCOORD2;
	float3		vOrientationY : TEXCOORD3;
	float3		vOrientationZ : TEXCOORD4;
};

struct PS_OUT_2D
{
	vector				vColor : COLOR0;
};


struct PS_IN_MESH
{
	float2	vTexUV : TEXCOORD0;
	vector	vProjPos : TEXCOORD1;
	float3	vOrientation : TEXCOORD2;
};

struct PS_OUT_MESH
{
	vector		vColor : COLOR0;
};

struct PS_FORWARD_INPUT 
{
	float2 mUV : TEXCOORD0;
	float3 mLightDir : TEXCOORD1;
	float3 mViewDir : TEXCOORD2;
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};


PS_OUT_2D PS_SOFTEFFECT(PS_IN_2D In)
{
	PS_OUT_2D		Out = (PS_OUT_2D)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= g_vColor;

	float2			vNewUV;

	// -1 -> 0, 1 -> 1
	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;

	// 1 -> 0, -1 -> 1
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepth = tex2D(DepthSampler, vNewUV);

	float vViewZ = vDepth.y * 1500.f;

	float fAlpha = saturate(vViewZ - In.vProjPos.w);


	Out.vColor.a = max(fAlpha, 0.f) * 2.f * Out.vColor.a;


	return Out;
}

PS_OUT_2D PS_2DBLEND(PS_IN_2D In)
{
	PS_OUT_2D		Out = (PS_OUT_2D)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= g_vColor;
	clamp(Out.vColor, 0.f, 1.f);		// 1 넘지마셈

	return Out;
}


PS_OUT_MESH PS_MESH(PS_IN_MESH In)
{
	PS_OUT_MESH		Out = (PS_OUT_MESH)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV * g_vUV);

	Out.vColor *= g_vColor;
	clamp(Out.vColor, 0.f, 1.f);

	return Out;
}

PS_OUT_MESH PS_MESHMASK(PS_FORWARD_INPUT Input)
{
	PS_OUT_MESH		Out = (PS_OUT_MESH)0.0f;

	vector vMaskColor = tex2D(MaskSampler, Input.mUV); 

	float3 lightColor = g_LightColor.rgb;

	// Normal
	float3 tangentNormal = tex2D(NormalSampler, Input.mUV * g_vUV).xyz;
	tangentNormal = normalize(tangentNormal * 2 - 1); // -1 ~ 1 사이로 변환.

	float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
	TBN = transpose(TBN);
	float3 worldNormal = mul(TBN, tangentNormal);

	float4 albedo = tex2D(BaseSampler, Input.mUV * g_vUV);

	albedo *= g_vColor; // 초록색 만들기.
	albedo *= vMaskColor; // 마스크맵 원하는 부위만 보이게.

	float3 lightDir = normalize(Input.mLightDir);
	float3 diffuse = saturate(dot(worldNormal, -lightDir));
	diffuse = lightColor * albedo * diffuse;

	float3 specular = 0;
	/*if (diffuse.x > 0)
	{
		float3 reflection = reflect(lightDir, worldNormal);
		float3 viewDir = normalize(Input.mViewDir);

		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 20.0f);
	}*/

	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;

	float4 ResultColor = float4(ambient + diffuse + specular, g_vColor.a);

	Out.vColor = ResultColor;
	return Out;
}

PS_OUT_MESH PS_MESHFOGMASK(PS_IN_MESH In)
{
	PS_OUT_MESH		Out = (PS_OUT_MESH)0.0f;
	///////////////////////////////////////////////////////////


	// X축 블러
	float4 Result = (float4)0;

	float2	t = In.vTexUV;
	float2	uv = 0;

	float	tu = 1. / m_TexW;

	for (int i = -6; i < 6; ++i)
	{
		uv = t + float2(tu *i, 0);
		Result += Weight[6 + i] * tex2D(MaskSampler, uv);
	}

	Result /= Total;


	// Y축 블러
	//float	tv = 1. / m_TexH;

	//for (int i = -6; i < 6; ++i)
	//{
	//	uv = t + float2(0, tv *i);
	//	Result += Weight[6 + i] * tex2D(MaskSampler, uv);
	//}

	//Result /= Total;
	///////////////////////////////////////////////////////////블러!


	Out.vColor = tex2D(BaseSampler, In.vTexUV * g_vUV);
	//vector vMaskColor = tex2D(MaskSampler, In.vTexUV * g_vUV);

	float2 FadeUV = In.vTexUV;
	FadeUV.y += g_fTime * fFadeSpeed;		// -는 연기가 내려가는 느낌 !
	vector vFadeColor = tex2D(MaskSampler, FadeUV);

	Out.vColor *= g_vColor * 1.5f;
	Out.vColor *= (Result.r * (vFadeColor.r));

	return Out;
}

float4 PS_DECAL(PS_IN_2D In) : COLOR0
{
	////////////////////////////////////////
	////http://ttmayrin.tistory.com/37
	////////////////////////////////////////

	// //옆면이 늘어나는것은 데칼상자의 투영방향과 법선방향이 이루는 각이 어느정도
	// //커지면 리젝션시키면됨.

	//float2 screenposition = In.vProjPos.xy/In.vProjPos.w;
	//screenposition.x = screenposition.x * 0.5 + 0.5;
	//screenposition.y = -screenposition.y * 0.5 + 0.5;

	//float depth = tex2D(DepthSampler, screenposition).y;

	//float3 normal = tex2D(NormalSampler, screenposition).xyz; 
	//normal = float3(normal * 2.f - 1.f); // 이걸 다시 -1 ~ 1사이로

	//clip(dot(normal, In.vOrientation) - 0.5f); // 0.5 => cos(60) 0.86f => cos(30)


	//float3 ViewRay = float3(lerp(-CameraRightTop.xy, CameraRightTop.xy, screenposition), CameraRightTop.z);
	//
	//float4 ViewPos = float4(ViewRay * depth, 1.f);

	//float4 ObjPos = mul(ViewPos, g_matViewInv);
	//ObjPos = mul(ObjPos, g_matWorldInv);

	//float3 ObjectAbs = abs(ObjPos.xyz);
	//clip(0.5f - ObjectAbs);

	//float2 uv = ObjPos.xz + 0.5f; //xy가 아닌 xz 
	//float4 color = tex2D(BaseSampler, uv);

	//float dist = abs(ObjPos.z);
	//float scaleDistance = max(dist * 2.0f, 1.0f);
	//float fadeOut = 1.0f - scaleDistance;

	//color.rgb *= g_vColor.rgb;

	//color.a *= fadeOut;

	//color *= (1.f - max((ObjectAbs.z - 0.25f) / 0.25f, 0.f));

	//// 옆면이 늘어나는것은 데칼상자의 투영방향과 법선방향이 이루는 각이 어느정도
	//// 커지면 리젝션시키면됨.

	//return color;

	float2 screenposition = In.vProjPos.xy/In.vProjPos.w;
	screenposition.x = screenposition.x * 0.5 + 0.5;
	screenposition.y = -screenposition.y * 0.5 + 0.5;

	float depth = tex2D(DepthSampler, screenposition).x;

	float3 normal = tex2D(NormalSampler, screenposition).xyz; 
	normal = float3(normal * 2.f - 1.f); // 이걸 다시 -1 ~ 1사이로


	float3 ViewRay = float3(lerp(-CameraRightTop.xy, CameraRightTop.xy, screenposition), CameraRightTop.z);
	
	float4 ViewPos = float4(ViewRay * depth, 1.f);

	float4 ObjPos = mul(ViewPos, g_matViewInv);
	ObjPos = mul(ObjPos, g_matWorldInv);

	float3 ObjectAbs = abs(ObjPos.xyz);
	clip(0.5f - ObjectAbs);


	// uv를 구할때! 어느면에 붙느냐에 따라서 예외적으로 생각해야함.
	// 픽셀의 노말값중 xyz값중 가장 큰값을 골라서 그 축에 해당하는 UV로 출력시키자.
	float2 uv;

	float3 PixelNormal = abs(normal);

	if (PixelNormal.y >= PixelNormal.x && PixelNormal.y >= PixelNormal.z)
	{
		uv = ObjPos.xz;
	}
	else if (PixelNormal.x >= PixelNormal.z && PixelNormal.x >= PixelNormal.y)
	{
		uv = ObjPos.zy;
	}
	else if (PixelNormal.z >= PixelNormal.x && PixelNormal.z >= PixelNormal.y)
	{
		uv = ObjPos.xy;
	}
	else
	{
		discard;
	}

	uv += 0.5f; // -0.5 ~ 0.5를 0 ~ 1로.


	//float2 uv = ObjPos.xz + 0.5f; //xy가 아닌 xz 
	float4 color = tex2D(BaseSampler, uv);
	float4 MaskColor = tex2D(MaskSampler, uv);

	//모든 축에 대해 반지름으로 계산한다.
	//float dist = abs(ObjPos.z);
	float dist = sqrt(ObjPos.x * ObjPos.x + ObjPos.y * ObjPos.y + ObjPos.z * ObjPos.z);

	float scaleDistance = max(dist * 2.0f, 1.0f);
	float fadeOut = 1.0f - scaleDistance;

	color.rgb *= g_vColor.rgb;

	color.a *= fadeOut;
	color *= (1.f - max((ObjectAbs.z - 0.25f) / 0.25f, 0.f));

	// 마스크맵을 넣어주장 ! 
	if(bMaskOn)
		color *= (MaskColor * fDecalMaskFadeIn);	// 스까
	else
		color *= fDecalMaskFadeIn;	// 스까

	//if (fBlendTime < 0.f)	// 섞는 시간이 시작되면 !
	//{
	//	//MaskColor.rgb *= vMaskColor.rgb;
	//	color *= (MaskColor * fDecalMaskFadeIn);	// 스까
	//}


	return color;
}

PS_OUT_MESH PS_MESHPROJECTILEMASK(PS_IN_MESH In)
{
	PS_OUT_MESH		Out = (PS_OUT_MESH)0.0f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV * g_vUV);
	float4 vMaskColor = tex2D(MaskSampler, In.vTexUV * g_vUV);

	Out.vColor *= g_vColor;
	Out.vColor.rgb *= vMaskColor.r * 3.f;
	//clamp(Out.vColor, 0.f, 1.f);

	return Out;
}


technique Default_Device
{
	pass EFFECT_2D_SOFTBLEND // 2D_알파블렌드+소프트이펙트
	{
		LIGHTING = false;

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = ONE;

		FILLMODE = SOLID;
		ZWRITEENABLE = TRUE;
		CULLMODE = none;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_SOFTEFFECT();
	}

	pass EFFECT_2D_ALPHABLEND		// 2D_알파블렌드
	{
		LIGHTING = false;

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;
		ZWRITEENABLE = TRUE;
		CULLMODE = none;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_2DBLEND();
	}

	pass EFFECT_ALPHABLEND_NO_Z	
	{
		LIGHTING = FALSE;
		
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;
		ZWRITEENABLE = FALSE;		// 흠..

		CULLMODE = NONE;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_2DBLEND();
	}

	pass EFFECT_RECTPARTICLE		// RECT_PARTICLE(Z버퍼 기록X)
	{
		LIGHTING = FALSE;
		
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;
		ZWRITEENABLE = FALSE;		// 흠..

		CULLMODE = NONE;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_2DBLEND();
	}

	pass EFFECT_RECTPARTICLE_INV		// RECT_PARTICLE(Z버퍼 기록X)
	{
		LIGHTING = FALSE;
		
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		FILLMODE = SOLID;
		ZWRITEENABLE = FALSE;		// 흠..

		CULLMODE = NONE;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_2DBLEND();
	}


	pass EFFECT_MESH_ALPHABLEND
	{
		LIGHTING = FALSE;

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;
		ZWRITEENABLE = FALSE;		// 흠..

		CULLMODE = CCW;

		VertexShader = compile vs_3_0 VS_MESHEFFECT();
		PixelShader = compile ps_3_0 PS_MESH();
	}

	pass EFFECT_MESH_ALPHABLEND_CULLNONE
	{
		LIGHTING = FALSE;
		
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;
		ZWRITEENABLE = FALSE;		// 흠..

		CULLMODE = NONE;

		VertexShader = compile vs_3_0 VS_MESHEFFECT();
		PixelShader = compile ps_3_0 PS_MESH();
	}

	pass EFFECT_MESH_ALPHATEST
	{
		LIGHTING = FALSE;

		ALPHABLENDENABLE = FALSE;

		ALPHATESTENABLE = TRUE;
		alphafunc = greater;
		FILLMODE = SOLID;
		alpharef = 0x00000008f;

		ZWRITEENABLE = TRUE;
		CULLMODE = CCW;

		VertexShader = compile vs_3_0 VS_MESHEFFECT();
		PixelShader = compile ps_3_0 PS_MESH();
	}

	pass EFFECT_MESH_MASK
	{
		LIGHTING = FALSE;

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;

		ZWRITEENABLE = FALSE;		// 흠..
		CULLMODE = CCW;

		//VertexShader = compile vs_3_0 VS_MESHEFFECT();
		VertexShader = compile vs_3_0 VS_BUMP_MAIN();
		PixelShader = compile ps_3_0 PS_MESHMASK();
	}

	pass EFFECT_MESH_FOGMASK
	{
		LIGHTING = FALSE;

		ALPHATESTENABLE = FALSE;
		
		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;

		ZWRITEENABLE = FALSE;		// Fog도 Z버퍼에 안쓰는게 맞는거같아.
		CULLMODE = none;


		VertexShader = compile vs_3_0 VS_MESHEFFECT();
		PixelShader = compile ps_3_0 PS_MESHFOGMASK();
	}

	pass EFFECT_DECAL
	{
		LIGHTING = FALSE;

		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;

		/*SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;*/

		/*SRCBLEND = SRCCOLOR;
		DESTBLEND = ONE;*/

		SRCBLEND = ONE;
		DESTBLEND = ONE;

		FILLMODE = SOLID;

		ZWRITEENABLE = FALSE;
		
		CULLMODE = CCW;

		VertexShader = compile vs_3_0 VS_2DEFFECT();
		PixelShader = compile ps_3_0 PS_DECAL();
	}

	pass EFFECT_MESH_PROJECTILE
	{
		LIGHTING = FALSE;
		ALPHATESTENABLE = FALSE;

		ALPHABLENDENABLE = TRUE;
		DESTBLEND = ONE;
		SRCBLEND = SRCALPHA;

		FILLMODE = SOLID;

		ZWRITEENABLE = FALSE;		// 흠..
		CULLMODE = none;

		VertexShader = compile vs_3_0 VS_MESHEFFECT();
		PixelShader = compile ps_3_0 PS_MESHPROJECTILEMASK();
	}
}