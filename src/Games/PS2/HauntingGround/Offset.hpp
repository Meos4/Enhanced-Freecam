#pragma once

#include "Common/Types.hpp"

namespace PS2::HauntingGround
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 Fn_memcpy,
			Fn_sceVu0CameraMatrix,
			Fn_sceVu0ScaleVectorXYZ,
			Fn_std__default_new_handler,
			adxFlags,
			cameraPtr,
			cutsceneState,
			sofdecVideoStructPtr,
			Fn_artGalleryPlayImage,
			Fn_blackBarCutscene,
			Fn_cameraTransition,
			Fn_cutsceneFrame,
			Fn_drawBlur,
			Fn_drawDynamicMap,
			Fn_drawEntity,
			Fn_drawFog,
			Fn_drawShade,
			Fn_drawStaticMap,
			Fn_drawSubtitles,
			Fn_drawTitleScreen,
			Fn_fionaMovementControls,
			Fn_initCostumeMenu,
			Fn_initMapProjection,
			Fn_isCharacterGalleryUnlocked,
			Fn_isParticleShouldntBeAnimated,
			Fn_isSofdecVideoRunning,
			Fn_musicGalleryPlayMusic,
			Fn_padCrossTriangle,
			Fn_padStatus,
			Fn_setFov,
			Fn_setMainMatrix,
			Fn_setMatrix,
			Fn_updateArtGallery,
			Fn_updateIngame,
			Fn_updateModelViewer,
			Fn_updateMovieGallery,
			Fn_updateMusicGallery,
			Fn_updatePanic,
			Fn_updateTitleScreen;
	};
}