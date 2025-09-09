#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::Sly1
{
	static constexpr Offset
	offsetPal
	{
		.g_pcm = 0x00262C90,
		.g_pjt = 0x00264070,
		.g_gsCur = 0x002859A0,
		.g_prompt = 0x00274E00,
		.g_fInvulnerable = 0x00263ECC,
		.Fn_BlendPrevFrame__Fv = 0x0015F7E8,
		.Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4 = 0x0018A5C0,
		.Fn_DrawAttract__FP7ATTRACT = 0x001AEF48,
		.Fn_DrawBinoc__FP5BINOC = 0x001367B8,
		.Fn_DrawBossctr__FP7BOSSCTR = 0x001ACEE0,
		.Fn_DrawCtr__FP3CTR = 0x001ABF98,
		.Fn_DrawLetterbox__FP9LETTERBOX = 0x001AE860,
		.Fn_DrawLogo__FP4LOGO = 0x001AEA90,
		.Fn_DrawNote__FP4NOTE = 0x001AD438,
		.Fn_DrawPrompt__FP6PROMPT = 0x00195BF0,
		.Fn_DrawTimer__FP5TIMER = 0x001AC528,
		.Fn_DrawTitle__FP5TITLE = 0x001AE138,
		.Fn_DrawTv__FP2TV = 0x001E83E8,
		.Fn_DrawWmc__FP3WMC = 0x001F30F0,
		.Fn_FInvulnerableJt__FP2JT3ZPK = 0x001D61C8,
		.Fn_FReadJoy__FP3JOY = 0x0016EF98,
		.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM = 0x0016ED90,
		.Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3 = 0x00189908,
		.Fn_MarkClockTick__FP5CLOCK = 0x00143210,
		.Fn_RecalcCmFrustrum__FP2CM = 0x00143860,
		.Fn_SetCm__FP2CMP6VECTORT1ff = 0x00145A18,
		.Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff = 0x00144EE8,
		.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 = 0x00144BC8,
		.Fn_UpdateCmMat4__FP2CM = 0x00144840,
		.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf = 0x00149A50,
		.levelInfo = 0x002490F0,
		.Fn_antiPiracy1 = 0x0015F280,
		.Fn_antiPiracy2 = 0x00190150,
		.Fn_antiPiracy3 = 0x001EABA8,
		.Fn_antiPiracy4 = 0x0012B880
	},
	offsetNtscU
	{
		.g_pcm = 0x00261990,
		.g_pjt = 0x00262E10,
		.g_gsCur = 0x0027C220,
		.g_prompt = 0x0026FA90,
		.g_fInvulnerable = 0x00262C64,
		.Fn_BlendPrevFrame__Fv = 0x0015F320,
		.Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4 = 0x00189F58,
		.Fn_DrawAttract__FP7ATTRACT = 0x001AE220,
		.Fn_DrawBinoc__FP5BINOC = 0x001366A0,
		.Fn_DrawBossctr__FP7BOSSCTR = 0x001AC0E8,
		.Fn_DrawCtr__FP3CTR = 0x001AB198,
		.Fn_DrawLetterbox__FP9LETTERBOX = 0x001ADB00,
		.Fn_DrawLogo__FP4LOGO = 0x001ADD28,
		.Fn_DrawNote__FP4NOTE = 0x001E0678,
		.Fn_DrawPrompt__FP6PROMPT = 0x00194F80,
		.Fn_DrawTimer__FP5TIMER = 0x001AB710,
		.Fn_DrawTitle__FP5TITLE = 0x001AD3F0,
		.Fn_DrawTv__FP2TV = 0x001E73B8,
		.Fn_DrawWmc__FP3WMC = 0x001F20A8,
		.Fn_FInvulnerableJt__FP2JT3ZPK = 0x001D54E0,
		.Fn_FReadJoy__FP3JOY = 0x0016E940,
		.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM = 0x0016E738,
		.Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3 = 0x001892A0,
		.Fn_MarkClockTick__FP5CLOCK = 0x00142F10,
		.Fn_RecalcCmFrustrum__FP2CM = 0x00143560,
		.Fn_SetCm__FP2CMP6VECTORT1ff = 0x00145718,
		.Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff = 0x00144BE8,
		.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 = 0x001448C8,
		.Fn_UpdateCmMat4__FP2CM = 0x00144540,
		.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf = 0x00149760,
		.levelInfo = 0x00247AF0,
		.Fn_antiPiracy1 = 0x0015EDB8,
		.Fn_antiPiracy2 = 0x0018F610,
		.Fn_antiPiracy3 = 0x001E9B30,
		.Fn_antiPiracy4 = 0x0012B760
	},
	offsetNtscJ
	{
		.g_pcm = 0x00261F10,
		.g_pjt = 0x00263320,
		.g_gsCur = 0x00284820,
		.g_prompt = 0x002740C0,
		.g_fInvulnerable = 0x0026317C,
		.Fn_BlendPrevFrame__Fv = 0x0015F878,
		.Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4 = 0x0018A860,
		.Fn_DrawAttract__FP7ATTRACT = 0x001AF720,
		.Fn_DrawBinoc__FP5BINOC = 0x001367A8,
		.Fn_DrawBossctr__FP7BOSSCTR = 0x001AD548,
		.Fn_DrawCtr__FP3CTR = 0x001AC600,
		.Fn_DrawLetterbox__FP9LETTERBOX = 0x001AEEC8,
		.Fn_DrawLogo__FP4LOGO = 0x001AF0C0,
		.Fn_DrawNote__FP4NOTE = 0x001ADAA0,
		.Fn_DrawPrompt__FP6PROMPT = 0x00196160,
		.Fn_DrawTimer__FP5TIMER = 0x001ACB90,
		.Fn_DrawTitle__FP5TITLE = 0x001AE7A0,
		.Fn_DrawTv__FP2TV = 0x001E8A90,
		.Fn_DrawWmc__FP3WMC = 0x001F3828,
		.Fn_FInvulnerableJt__FP2JT3ZPK = 0x001D6828,
		.Fn_FReadJoy__FP3JOY = 0x0016F220,
		.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM = 0x0016F018,
		.Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3 = 0x00189BA8,
		.Fn_MarkClockTick__FP5CLOCK = 0x001432B0,
		.Fn_RecalcCmFrustrum__FP2CM = 0x00143900,
		.Fn_SetCm__FP2CMP6VECTORT1ff = 0x00145AB8,
		.Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff = 0x00144F88,
		.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 = 0x00144C68,
		.Fn_UpdateCmMat4__FP2CM = 0x001448E0,
		.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf = 0x00149AF8,
		.levelInfo = 0x00249330,
		.Fn_antiPiracy1 = 0x0015F310,
		.Fn_antiPiracy2 = 0x00190660,
		.Fn_antiPiracy3 = 0x001EB2E0,
		.Fn_antiPiracy4 = 0x0012B880
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetPal,
			&offsetNtscU,
			&offsetNtscJ
		};

		return *offsets[version];
	}
}