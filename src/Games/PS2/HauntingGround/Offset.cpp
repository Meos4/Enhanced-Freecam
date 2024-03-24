#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::HauntingGround
{
	static constexpr Offset
	offsetPal
	{
		.Fn_memcpy = 0x00115B68,
		.Fn_sceVu0CameraMatrix = 0x0010E0E0,
		.Fn_sceVu0ScaleVectorXYZ = 0x0010E640,
		.Fn_std__default_new_handler = 0x00100570,
		.adxFlags = 0x003CFD04,
		.cameraPtr = 0x00450738,
		.cutsceneState = 0x017F6224,
		.sofdecVideoStructPtr = 0x003E89CC,
		.Fn_artGalleryPlayImage = 0x0034AD20,
		.Fn_blackBarCutscene = 0x0035E620,
		.Fn_cameraTransition = 0x001F6CF0,
		.Fn_cutsceneFrame = 0x0033F150,
		.Fn_drawBlur = 0x002935C0,
		.Fn_drawDynamicMap = 0x00230F70,
		.Fn_drawEntity = 0x00287DB0,
		.Fn_drawFog = 0x0028BEC0,
		.Fn_drawShade = 0x0023A4E0,
		.Fn_drawStaticMap = 0x00230180,
		.Fn_drawSubtitles = 0x00347AF0,
		.Fn_drawTitleScreen = 0x0038BF00,
		.Fn_fionaMovementControls = 0x0016D990,
		.Fn_initCostumeMenu = 0x003537A0,
		.Fn_initMapProjection = 0x001F7340,
		.Fn_isCharacterGalleryUnlocked = 0x00351F40,
		.Fn_isParticleShouldntBeAnimated = 0x00382250,
		.Fn_isSofdecVideoRunning = 0x00226490,
		.Fn_musicGalleryPlayMusic = 0x0034B6F0,
		.Fn_padCrossTriangle = 0x003405A0,
		.Fn_padStatus = 0x0029CD40,
		.Fn_setFov = 0x00122030,
		.Fn_setMainMatrix = 0x001F6CB0,
		.Fn_setMatrix = 0x001225F0,
		.Fn_updateArtGallery = 0x00349510,
		.Fn_updateIngame = 0x00386550,
		.Fn_updateModelViewer = 0x0034D920,
		.Fn_updateMovieGallery = 0x00352070,
		.Fn_updateMusicGallery = 0x0034B220,
		.Fn_updatePanic = 0x00384850,
		.Fn_updateTitleScreen = 0x0038CA90
	},
	offsetNtscU
	{
		.Fn_memcpy = 0x00115B68,
		.Fn_sceVu0CameraMatrix = 0x0010E0E0,
		.Fn_sceVu0ScaleVectorXYZ = 0x0010E640,
		.Fn_std__default_new_handler = 0x00100570,
		.adxFlags = 0x003CEC14,
		.cameraPtr = 0x0044E4B8,
		.cutsceneState = 0x017F3D24,
		.sofdecVideoStructPtr = 0x003E897C,
		.Fn_artGalleryPlayImage = 0x00387F00,
		.Fn_blackBarCutscene = 0x002C9EA0,
		.Fn_cameraTransition = 0x00224C60,
		.Fn_cutsceneFrame = 0x002B6510,
		.Fn_drawBlur = 0x002C6650,
		.Fn_drawDynamicMap = 0x0025F0A0,
		.Fn_drawEntity = 0x002B72D0,
		.Fn_drawFog = 0x002BB3E0,
		.Fn_drawShade = 0x002685F0,
		.Fn_drawStaticMap = 0x0025E2B0,
		.Fn_drawSubtitles = 0x00384B60,
		.Fn_drawTitleScreen = 0x0012DDB0,
		.Fn_fionaMovementControls = 0x00187650,
		.Fn_initCostumeMenu = 0x0038F7D0,
		.Fn_initMapProjection = 0x002252B0,
		.Fn_isCharacterGalleryUnlocked = 0x0038DF90,
		.Fn_isParticleShouldntBeAnimated = 0x0039A7C0,
		.Fn_isSofdecVideoRunning = 0x002545C0,
		.Fn_musicGalleryPlayMusic = 0x003888A0,
		.Fn_padCrossTriangle = 0x0037E320,
		.Fn_padStatus = 0x002D4780,
		.Fn_setFov = 0x00122030,
		.Fn_setMainMatrix = 0x00224C20,
		.Fn_setMatrix = 0x001225F0,
		.Fn_updateArtGallery = 0x00386550,
		.Fn_updateIngame = 0x0039EAB0,
		.Fn_updateModelViewer = 0x0038A990,
		.Fn_updateMovieGallery = 0x0038E0C0,
		.Fn_updateMusicGallery = 0x00388400,
		.Fn_updatePanic = 0x0039CDC0,
		.Fn_updateTitleScreen = 0x0012E8C0
	},
	offsetNtscJ
	{
		.Fn_memcpy = 0x00115B68,
		.Fn_sceVu0CameraMatrix = 0x0010E0E0,
		.Fn_sceVu0ScaleVectorXYZ = 0x0010E640,
		.Fn_std__default_new_handler = 0x00100570,
		.adxFlags = 0x003CEC94,
		.cameraPtr = 0x0044E538,
		.cutsceneState = 0x017F3DA4,
		.sofdecVideoStructPtr = 0x003E89FC,
		.Fn_artGalleryPlayImage = 0x0038FF10,
		.Fn_blackBarCutscene = 0x002C9EA0,
		.Fn_cameraTransition = 0x00224C60,
		.Fn_cutsceneFrame = 0x002B6510,
		.Fn_drawBlur = 0x002C6650,
		.Fn_drawDynamicMap = 0x0025F0A0,
		.Fn_drawEntity = 0x002B72D0,
		.Fn_drawFog = 0x002BB3E0,
		.Fn_drawShade = 0x002685F0,
		.Fn_drawStaticMap = 0x0025E2B0,
		.Fn_drawSubtitles = 0x0038CB90,
		.Fn_drawTitleScreen = 0x0012DDB0,
		.Fn_fionaMovementControls = 0x00187650,
		.Fn_initCostumeMenu = 0x003977A0,
		.Fn_initMapProjection = 0x002252B0,
		.Fn_isCharacterGalleryUnlocked = 0x00395F60,
		.Fn_isParticleShouldntBeAnimated = 0x0037EEC0,
		.Fn_isSofdecVideoRunning = 0x002545C0,
		.Fn_musicGalleryPlayMusic = 0x003908B0,
		.Fn_padCrossTriangle = 0x00386310,
		.Fn_padStatus = 0x002D4780,
		.Fn_setFov = 0x00122030,
		.Fn_setMainMatrix = 0x00224C20,
		.Fn_setMatrix = 0x001225F0,
		.Fn_updateArtGallery = 0x0038E560,
		.Fn_updateIngame = 0x00383200,
		.Fn_updateModelViewer = 0x003929A0,
		.Fn_updateMovieGallery = 0x00396090,
		.Fn_updateMusicGallery = 0x00390410,
		.Fn_updatePanic = 0x00381510,
		.Fn_updateTitleScreen = 0x0012E8C0
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