#include "State.hpp"

#include "AsyncGameSearcher.hpp"
#include "Console.hpp"
#include "EfException.hpp"
#include "GameInfo.hpp"
#include "GamePlatform.hpp"
#include "GamePlatformWrapper.hpp"
#include "Path.hpp"
#include "Process.hpp"
#include "Util.hpp"
#include "Renderer.hpp"
#include "Settings.hpp"
#include "Types.hpp"
#include "Ui.hpp"

#include <format>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace State
{
	enum class Id
	{
		WaitingProcess,
		SearchGame,
		Running
	};

	static struct
	{
		State::Id state{ State::Id::WaitingProcess };
		GamePlatformWrapper gpw{ 0 };
		std::vector<std::unique_ptr<Process>> processList;
		std::shared_ptr<Process> process;
		std::unique_ptr<AsyncGameSearcher> ags;
		std::unique_ptr<GameLoop> gameLoop;
		float searchProcessTime{};
		float invalidGameTime{};
	} context;

	static constexpr auto
		_base{ "base" },
		_State{ "State" },
		_platform{ "platform" },
		_game{ "game" },
		_version{ "version" };

	static void transitionDecrease(State::Id state)
	{
		if (state <= State::Id::SearchGame)
		{
			if (context.ags)
			{
				context.ags->stop();
			}
			context.gameLoop.reset();
		}
		if (state == State::Id::WaitingProcess)
		{
			context.process.reset();
			context.searchProcessTime = {};
		}

		context.state = state;
	}

	static void transitionToSearchGame(std::unique_ptr<Process>&& process)
	{
		State::transitionDecrease(State::Id::SearchGame);
		context.state = State::Id::SearchGame;
		context.process = std::move(process);
	}

	static void transitionToRunning(std::unique_ptr<GameLoop>&& gameLoop)
	{
		context.state = State::Id::Running;
		context.gameLoop = std::move(gameLoop);
		context.invalidGameTime = {};
	}

	static bool isAUsableProcess(const Process& process)
	{
		auto processNameLowered{ process.name() };
		Util::toLower(&processNameLowered);

		if (context.gpw.isAnEmulator())
		{
			for (const auto& [name, pattern] : context.gpw.emulators())
			{
				std::string patternLowered{ pattern };
				Util::toLower(&patternLowered);
				if (processNameLowered.find(patternLowered) != std::string::npos)
				{
					return true;
				}
			}
		}
		else
		{
			throw EfException{ "Not implemented" };
		}

		return false;
	}

	static bool isProcessValid(const Process& process)
	{
		return process.isValid() && (g_settings.processSearchMode != SearchMode::Auto || State::isAUsableProcess(process));
	}

	static void updateWaitingProcess()
	{
		const auto rendererTime{ Renderer::time() };

		if (g_settings.processSearchMode != SearchMode::Auto || rendererTime < context.searchProcessTime)
		{
			return;
		}

		context.searchProcessTime = rendererTime + g_settings.searchProcessDelay;

		for (auto pId : Process::taskBarPId())
		{
			auto process{ Process::create(pId) };

			if (process && State::isAUsableProcess(*process))
			{
				State::transitionToSearchGame(std::move(process));
				return;
			}
		}
	}

	static void updateSearchGame()
	{
		if (!State::isProcessValid(*context.process))
		{
			State::transitionDecrease(State::Id::WaitingProcess);
			return;
		}

		if (!context.ags)
		{
			const auto processJson{ std::format("{}.json", context.process->name().c_str()) };

			try
			{
				const auto json{ Json::read(Path::settingsProcessFile(context.gpw.settingsName(), processJson.c_str())) };

				if (json.has_value() && json.value().contains(_base))
				{
					const auto& gameInfo{ context.gpw.gameInfo() };
					const auto op{ gameInfo.offsetPattern(context.gpw.version()) };
					const std::uintptr_t begin{ json.value()[_base] };

					if (Util::isValidProcessPatternOffset(*context.process, begin + op.offset, op.pattern))
					{
						State::transitionToRunning(gameInfo.createLoop(context.gpw.createRam(context.process, begin), context.gpw.version()));
						return;
					}
				}
			}
			catch (const Json::Exception& e)
			{
				Console::append(Console::Type::Exception, Json::exceptionFormat, processJson, e.what());
			}

			context.ags = std::make_unique<AsyncGameSearcher>(context.process, context.gpw.gameInfo().offsetPattern(context.gpw.version()), context.gpw.agsCallback());
		}
		else
		{
			const auto agsGet{ context.ags->get() };

			if (agsGet.has_value())
			{
				if (agsGet != AsyncGameSearcher::exitValue)
				{
					Json::Write json;
					Json::set(&json, _base, agsGet.value());
					const auto processJson{ std::format("{}.json", context.process->name().c_str()) };
					Json::overwrite(json, Path::settingsProcessFile(context.gpw.settingsName(), processJson.c_str()));
					State::transitionToRunning(context.gpw.gameInfo().createLoop(context.gpw.createRam(context.process, agsGet.value()), context.gpw.version()));
				}

				context.ags.reset();
			}
		}
	}

	static void updateRunning()
	{
		if (!State::isProcessValid(*context.process))
		{
			State::transitionDecrease(State::Id::WaitingProcess);
			return;
		}

		if (context.gameLoop->isValid())
		{
			context.invalidGameTime = {};
			return;
		}

		if (!context.gpw.isAnEmulator())
		{
			State::transitionDecrease(State::Id::SearchGame);
			return;
		}

		// On emulator the game becomes invalid for a short delay
		// due to save states and has to wait a small delay

		const auto rendererTime{ Renderer::time() };

		if (context.invalidGameTime == 0.f)
		{
			context.invalidGameTime = rendererTime + g_settings.invalidGameDelay;
		}

		if (rendererTime > context.invalidGameTime)
		{
			State::transitionDecrease(State::Id::SearchGame);
		}
	}

	void drawWindow()
	{
		ImGui::Begin(_State);
		
		const auto& style{ ImGui::GetStyle() };
		const auto maximumWidthSize{ 512.f - style.WindowPadding.x };

		ImGui::SetNextItemWidth(maximumWidthSize * 0.6f);
		if (ImGui::BeginCombo("##Platform names", context.gpw.platformName(), ImGuiComboFlags_None))
		{
			for (s32 i{}; i < GamePlatform::Count; ++i)
			{
				if (ImGui::Selectable(GamePlatform::name(i)) && context.gpw.platform() != i)
				{
					context.gpw.setPlatform(i);

					if (g_settings.processSearchMode == SearchMode::Auto)
					{
						State::transitionDecrease(State::Id::WaitingProcess);
					}
					else if (context.process)
					{
						State::transitionDecrease(State::Id::SearchGame);
					}
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SetNextItemWidth(maximumWidthSize * 0.6f);
		if (ImGui::BeginCombo("##Game names", context.gpw.gameName(), ImGuiComboFlags_None))
		{
			const auto gamesInfo{ context.gpw.gamesInfo() };
			for (s32 i{}; i < gamesInfo.size(); ++i)
			{
				if (ImGui::Selectable(gamesInfo[i].name) && context.gpw.game() != i)
				{
					context.gpw.setGame(i);

					if (!context.process || (!context.gpw.isAnEmulator() && g_settings.processSearchMode == SearchMode::Auto))
					{
						State::transitionDecrease(State::Id::WaitingProcess);
					}
					else
					{
						State::transitionDecrease(State::Id::SearchGame);
					}
				}
			}
			
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(maximumWidthSize * 0.4f - style.ItemSpacing.x);
		if (ImGui::BeginCombo("##Version names", context.gpw.versionName(), ImGuiComboFlags_None))
		{
			const auto& gameInfo{ context.gpw.gameInfo() };
			for (s32 i{}; i < gameInfo.count; ++i)
			{
				if (ImGui::Selectable(gameInfo.versionText(i)) && context.gpw.version() != i)
				{
					context.gpw.setVersion(i);

					if (context.process)
					{
						State::transitionDecrease(State::Id::SearchGame);
					}
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, context.gameLoop ? Ui::color(Ui::Color::Success) : Ui::color(Ui::Color::Error));
		if (g_settings.processSearchMode == SearchMode::Auto)
		{
			if (context.process)
			{
				ImGui::TextUnformatted(context.process->name().c_str());
				ImGui::PopStyleColor();
			}
			else
			{
				const bool isAnEmulator{ context.gpw.isAnEmulator() };
				ImGui::TextUnformatted(isAnEmulator ? "Emulator not found" : "Process not found");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				std::string supported{ isAnEmulator ? "Supported emulator:" : "Supported process:" };

				if (isAnEmulator)
				{
					for (const auto& [name, pattern] : context.gpw.emulators())
					{
						supported += std::format("\n- {}", name);
					}
				}
				else
				{
					throw EfException{ "Not implemented" };
				}

				ImGui::TextDisabled("(?)");
				Ui::hoveredTooltip(supported.c_str());
			}
		}
		else
		{
			auto processInfoFormatted = [](const Process& process)
			{
				return std::format("{} ({}) ({} bits)",
					process.name(),
					process.pId(),
					process.architecture() == Process::Architecture::x86 ? "32" : "64");
			};

			ImGui::SetNextItemWidth(maximumWidthSize);
			if (ImGui::BeginCombo("##Process Names", context.process ? processInfoFormatted(*context.process).c_str() : "", ImGuiComboFlags_None))
			{
				ImGui::PopStyleColor();
				if (context.processList.empty())
				{
					context.processList = Process::taskBarProcessList();
				}

				for (auto& process : context.processList)
				{
					if (ImGui::Selectable(processInfoFormatted(*process).c_str()) && !(context.process && context.process->pId() == process->pId()))
					{
						State::transitionToSearchGame(std::move(process));
						context.processList.clear();
						break;
					}
				}

				ImGui::EndCombo();
			}
			else
			{
				ImGui::PopStyleColor();
				context.processList.clear();
			}
		}

		ImGui::End();
	}

	void update()
	{
		switch (context.state)
		{
		case State::Id::WaitingProcess: State::updateWaitingProcess(); break;
		case State::Id::SearchGame: State::updateSearchGame(); break;
		case State::Id::Running: State::updateRunning(); break;
		}
	}

	void destroy()
	{
		context.process.reset();
		context.ags.reset();
		context.gameLoop.reset();
	}

	GameLoop* game()
	{
		return context.gameLoop.get();
	}

	void readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_State))
			{
				const auto& j{ json[_State] };

				s32 platform{};
				Json::get(j, _platform, &platform, 0, GamePlatform::Count - 1);
				context.gpw.setPlatform(platform);

				s32 game{};
				Json::get(j, _game, &game, 0, static_cast<s32>(context.gpw.gamesInfo().size() - 1));
				context.gpw.setGame(game);

				s32 version{};
				Json::get(j, _version, &version, 0, context.gpw.gameInfo().count - 1);
				context.gpw.setVersion(version);
			}
		}
		catch (const Json::Exception& e)
		{
			Console::append(Console::Type::Exception, Json::exceptionFormat, _State, e.what());
		}
	}

	void writeSettings(Json::Write* json)
	{
		auto* const j{ &(*json)[_State] };
		Json::set(j, _platform, context.gpw.platform());
		Json::set(j, _game, context.gpw.game());
		Json::set(j, _version, context.gpw.version());
	}
}