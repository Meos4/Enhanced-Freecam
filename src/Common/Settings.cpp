#include "Settings.hpp"

#include "Fonts/Roboto-Medium.hpp"

#include "Console.hpp"
#include "GamepadWrapper.hpp"
#include "Path.hpp"
#include "Types.hpp"
#include "Ui.hpp"

#include "imgui_internal.h"

#include <cstring>

Settings g_settings;

static constexpr auto _Settings{ "Settings" };
static constexpr auto decimalsMax{ 6 };

void Settings::init()
{
	const auto imguiIniPath{ Path::imguiIni() };
	ImGui::LoadIniSettingsFromDisk(imguiIniPath.string().c_str());

	auto* const style{ &ImGui::GetStyle() };

	style->FrameRounding = 4.f;
	style->GrabRounding = 4.f;
	style->ScrollbarRounding = 4.f;
	style->GrabMinSize = 8.f;
	style->TabBorderSize = 1.f;
	style->DockingSeparatorSize = 1.f;

	updateTheme();

	auto* const io{ &ImGui::GetIO() };
	io->IniFilename = NULL;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->Fonts->AddFontFromMemoryCompressedTTF(Roboto_Medium_compressed_data, Roboto_Medium_compressed_size, 13.f);
}

void Settings::drawWindow()
{
	ImGui::Begin("System");

	Ui::setXSpacingStr("PCSX2 Cheats Path");

	auto radioButtonsChoice = [](const char* label, void* v, const char* choice1, const char* choice2)
	{
		Ui::labelXSpacing(label);
		if (ImGui::BeginTable(label, 3))
		{
			ImGui::TableNextColumn();
			ImGui::RadioButton(choice1, (s32*)v, 0);
			ImGui::TableNextColumn();
			ImGui::RadioButton(choice2, (s32*)v, 1);
			ImGui::EndTable();
		}
	};

	auto decimalsFloatSlider = [](const char* label, s32* digits)
	{
		Ui::slider(Ui::lol(label), digits, "%d", ImGuiSliderFlags_AlwaysClamp, 0, 6);
	};

	if (ImGui::BeginTabBar("SystemTabBar", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Ui"))
		{
			if (ImGui::Combo(Ui::lol("Theme"), (s32*)&theme, "Dark\0Ruby\0Dark 2\0Light\0Classic\0"))
			{
				updateTheme();
			}

			if (Ui::checkbox(Ui::lol("Multi Viewports"), &isMultiViewports))
			{
				updateIsMultiViewports();
			}
			Ui::hoveredTooltip("Allows you to move windows outside the main window");

			ImGui::BeginDisabled(!isMultiViewports);
			if (Ui::checkbox(Ui::lol("Always On Top"), &multiViewportsAlwaysOnTop))
			{
				updateMultiViewportsAlwaysOnTop();
			}
			Ui::hoveredTooltip("Windows outside the main window remain focused on the screen");
			ImGui::EndDisabled();

			Ui::labelXSpacing("Welcome Message");
			if (Ui::buttonItemWidth("Open"))
			{
				isWelcomeModalOpen = !isWelcomeModalOpen;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Freecam"))
		{
			Ui::checkbox(Ui::lol("Smooth Camera"), &smoothCamera);
			Ui::hoveredTooltip("Moves the camera smoothly");
			decimalsFloatSlider("Position Decimals", &positionFloatDecimals);
			decimalsFloatSlider("Rotation Decimals", &rotationFloatDecimals);
			decimalsFloatSlider("Fov Decimals", &fovFloatDecimals);
			decimalsFloatSlider("Settings Decimals", &freecamSettingsFloatDecimals);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Input"))
		{
			Ui::separatorText("Gamepad");
			Ui::labelXSpacing("Connected");
			const auto& itemSpacing{ ImGui::GetStyle().ItemSpacing };
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { itemSpacing.x, itemSpacing.y + 1.f });

			for (u32 i{}; i < GamepadWrapper::gamepadMax; ++i)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, Ui::color(GamepadWrapper::isActive(i) ? Ui::Color::Success : Ui::Color::Error));
				ImGui::Text("Gamepad %d", i + 1);
				ImGui::PopStyleColor();

				if (i != GamepadWrapper::gamepadMax - 1)
				{
					ImGui::SameLine();
				}
			}

			ImGui::PopStyleVar();

			auto deadzoneSlider = [](const char* label, float* val)
			{
				Ui::slider(Ui::lol(label), val, "%.4f", ImGuiSliderFlags_AlwaysClamp, 0.f, 1.f);
			};

			deadzoneSlider("Stick Deadzone", &gamepad.stickDeadzone);
			deadzoneSlider("Trigger Deadzone", &gamepad.triggerDeadzone);
			radioButtonsChoice("Layout", &gamepadLayout, "Xbox", "PlayStation");

			Ui::separatorText("Keyboard");
			radioButtonsChoice("Layout", &keyboardLayout, "Qwerty", "Azerty");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Advanced"))
		{
			radioButtonsChoice("Process", &processSearchMode, "Auto", "Manual");
			Ui::hoveredTooltip("Allow you to choose the process, only use if you know what you are doing");

			Ui::checkbox(Ui::lol("PCSX2 Cheats Path"), &pcsx2.useDifferentCheatsPath);
			Ui::hoveredTooltip("Use a different PCSX2 cheats path, otherwise it use \"PCSX2 Directory/cheats\" path");
			static constexpr auto pathMaxSize{ 256 };
			pcsx2.cheatsPath.resize(pathMaxSize);
			ImGui::SameLine();
			ImGui::BeginDisabled(!pcsx2.useDifferentCheatsPath);
			ImGui::SetNextItemWidth(ImGui::CalcItemWidth() + 64.f);
			ImGui::InputText("##PCSX2CheatsPath", pcsx2.cheatsPath.data(), pathMaxSize);
			ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

bool Settings::isADarkTheme()
{
	return theme != Theme::Light;
}

void Settings::readSettings(const Json::Read& json)
{
	try
	{
		if (json.contains(_Settings))
		{
			const auto& j{ json[_Settings] };

			JSON_GET_MIN_MAX(j, theme, 0, static_cast<s32>(Theme::Count) - 1);
			updateTheme();
			JSON_GET(j, isMultiViewports);
			updateIsMultiViewports();
			JSON_GET(j, multiViewportsAlwaysOnTop);
			updateMultiViewportsAlwaysOnTop();

			JSON_GET(j, smoothCamera);
			JSON_GET_MIN_MAX(j, positionFloatDecimals, 0, decimalsMax);
			JSON_GET_MIN_MAX(j, rotationFloatDecimals, 0, decimalsMax);
			JSON_GET_MIN_MAX(j, fovFloatDecimals, 0, decimalsMax);
			JSON_GET_MIN_MAX(j, freecamSettingsFloatDecimals, 0, decimalsMax);
			JSON_GET_MIN_MAX(j, movementSpeed, movementSpeedMin, movementSpeedMax);
			JSON_GET_MIN_MAX(j, rotationSpeed, rotationSpeedMin, rotationSpeedMax);

			JSON_GET_MIN_MAX(j, gamepad.stickDeadzone, 0.f, 1.f);
			JSON_GET_MIN_MAX(j, gamepad.triggerDeadzone, 0.f, 1.f);
			JSON_GET_MIN_MAX(j, gamepadLayout, 0, 1);
			JSON_GET_MIN_MAX(j, keyboardLayout, 0, 1);

			JSON_GET_MIN_MAX(j, processSearchMode, 0, 1);
			JSON_GET(j, pcsx2.useDifferentCheatsPath);
			if (j.contains("pcsx2.cheatsPath"))
			{
				pcsx2.cheatsPath = j["pcsx2.cheatsPath"].get<std::string>();
			}
			JSON_GET(j, isWelcomeModalOpen);
		}
	}
	catch (const Json::Exception& e)
	{
		Console::append(Console::Type::Exception, Json::exceptionFormat, _Settings, e.what());
	}
}

void Settings::writeSettings(Json::Write* json) const
{
	const auto imguiIniPath{ Path::imguiIni() };
	ImGui::SaveIniSettingsToDisk(imguiIniPath.string().c_str());

	auto* const j{ &(*json)[_Settings] };
	JSON_SET(j, theme);
	JSON_SET(j, isMultiViewports);
	JSON_SET(j, multiViewportsAlwaysOnTop);

	JSON_SET(j, smoothCamera);
	JSON_SET(j, positionFloatDecimals);
	JSON_SET(j, rotationFloatDecimals);
	JSON_SET(j, fovFloatDecimals);
	JSON_SET(j, freecamSettingsFloatDecimals);
	JSON_SET(j, movementSpeed);
	JSON_SET(j, rotationSpeed);

	JSON_SET(j, gamepad.stickDeadzone);
	JSON_SET(j, gamepad.triggerDeadzone)
	JSON_SET(j, gamepadLayout);
	JSON_SET(j, keyboardLayout);

	JSON_SET(j, processSearchMode);
	JSON_SET(j, pcsx2.useDifferentCheatsPath);
	auto pcsx2CheatsPathNoZeros{ pcsx2.cheatsPath };
	pcsx2CheatsPathNoZeros.resize(std::strlen(pcsx2.cheatsPath.c_str()));
	Json::set(j, "pcsx2.cheatsPath", pcsx2CheatsPathNoZeros);
	JSON_SET(j, isWelcomeModalOpen);
}

std::array<float, 3> Settings::rgbFont() const
{
	const auto& v{ ImGui::GetStyle().Colors[ImGuiCol_WindowBg] };
	return { v.x, v.y, v.z };
}

void Settings::updateTheme() const
{
	auto* const style{ &ImGui::GetStyle() };

	switch (theme)
	{
	case Theme::Dark: Settings::setThemeDark(); style->FrameBorderSize = 1.f; return;
	case Theme::Ruby: Settings::setThemeRuby(); style->FrameBorderSize = 1.f; return;
	case Theme::Dark2: ImGui::StyleColorsDark(); style->FrameBorderSize = 0.f; return;
	case Theme::Light: ImGui::StyleColorsLight(); style->FrameBorderSize = 1.f; return;
	case Theme::Classic: ImGui::StyleColorsClassic(); style->FrameBorderSize = 0.f; return;
	}
}

void Settings::updateIsMultiViewports() const
{
	auto* const configFlags{ &ImGui::GetIO().ConfigFlags };
	isMultiViewports ? *configFlags |= ImGuiConfigFlags_ViewportsEnable : *configFlags &= ~ImGuiConfigFlags_ViewportsEnable;
}

void Settings::updateMultiViewportsAlwaysOnTop() const
{
	const auto& g{ *GImGui };
	for (s32 i{ 1 }; i < g.Viewports.Size; ++i)
	{
		ImGui::DestroyPlatformWindow(g.Viewports[i]);
	}
}

void Settings::setThemeDark()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.20f, 0.20f, 0.20f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Settings::setThemeRuby()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.09f, 0.11f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.04f, 0.04f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.26f, 0.26f, 0.50f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.47f, 0.16f, 0.16f, 0.50f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.47f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.47f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.78f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.63f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.71f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.47f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.47f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.47f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.09f, 0.10f, 0.11f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.55f, 0.55f, 0.56f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.21f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.24f, 0.25f, 0.26f, 0.25f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.47f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.24f, 0.26f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.47f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.71f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.71f, 0.26f, 0.26f, 0.50f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.71f, 0.26f, 0.26f, 0.71f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);
}