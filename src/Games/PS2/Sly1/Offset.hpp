#pragma once

#include "Common/Types.hpp"

namespace PS2::Sly1
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		// E3 5-19D
		u32 g_pcm,
			g_pjt,
			g_gsCur,
			g_prompt,
			g_fInvulnerable,
			Fn_BlendPrevFrame__Fv,
			Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4,
			Fn_DrawAttract__FP7ATTRACT,
			Fn_DrawBinoc__FP5BINOC,
			Fn_DrawBossctr__FP7BOSSCTR,
			Fn_DrawCtr__FP3CTR,
			Fn_DrawLetterbox__FP9LETTERBOX,
			Fn_DrawLogo__FP4LOGO,
			Fn_DrawNote__FP4NOTE,
			Fn_DrawPrompt__FP6PROMPT,
			Fn_DrawTimer__FP5TIMER,
			Fn_DrawTitle__FP5TITLE,
			Fn_DrawTv__FP2TV,
			Fn_DrawWmc__FP3WMC,
			Fn_FInvulnerableJt__FP2JT3ZPK,
			Fn_FReadJoy__FP3JOY,
			Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM,
			Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3,
			Fn_MarkClockTick__FP5CLOCK,
			Fn_RecalcCmFrustrum__FP2CM,
			Fn_SetCm__FP2CMP6VECTORT1ff,
			Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff,
			Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3,
			Fn_UpdateCmMat4__FP2CM,
			Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf,
			//
			levelInfo,
			Fn_antiPiracy1,
			Fn_antiPiracy2,
			Fn_antiPiracy3,
			Fn_antiPiracy4;
	};
}