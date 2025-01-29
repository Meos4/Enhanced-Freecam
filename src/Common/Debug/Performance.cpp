#include "Performance.hpp"

#if EF_DEBUG
#include "Common/Renderer.hpp"
#include "Common/Ui.hpp"

#include <chrono>
#include <format>
#include <unordered_map>
#include <vector>

namespace Debug::Performance
{
	static constexpr auto refreshRateDefault{ 60 };

	static auto refreshRate()
	{
		const auto rr{ Renderer::refreshRate() };
		return (rr.has_value() && rr.value() > 0) ? rr.value() : refreshRateDefault;
	}

	class Chrono final
	{
	public:
		void start()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		float elapsedMs() const
		{
			std::chrono::duration<float> duration{ std::chrono::high_resolution_clock::now() - m_start };
			return duration.count() * 1000.f;
		}
	private:
		std::chrono::steady_clock::time_point m_start{ std::chrono::high_resolution_clock::now() };
	};

	struct PlotLines
	{
		Chrono chrono;
		std::vector<float> elapsedMs;
		std::size_t index;
		bool isActive;
	};

	static std::unordered_map<const char*, PlotLines> plotLines;

	void draw()
	{
		auto avgTotal{ 0.f };
		const auto refreshRateMs{ 1000.f / Debug::Performance::refreshRate() };

		for (const auto& [name, perf] : plotLines)
		{
			auto avg{ 0.f };
			const auto elapsedMsSize{ perf.elapsedMs.size() };

			for (auto ms : perf.elapsedMs)
			{
				avg += ms;
			}

			if (avg)
			{
				avg /= elapsedMsSize;
				avgTotal += avg;
			}

			const auto overlay{ std::format("{:.2f} ms", avg) };
			ImGui::PlotLines(Ui::lol(name), perf.elapsedMs.data(), static_cast<s32>(elapsedMsSize), 0, overlay.c_str(), 0.f, refreshRateMs, { 0, 40.f });
		}

		Ui::labelXSpacing("Average");
		ImGui::Text("%.2f", avgTotal);
	}

	void update()
	{
		std::vector<const char*> inactive;

		for (const auto& [name, perf] : plotLines)
		{
			if (!perf.isActive)
			{
				inactive.emplace_back(name);
			}
		}

		for (auto name : inactive)
		{
			plotLines.erase(name);
		}

		const auto refreshRate{ Debug::Performance::refreshRate() };

		for (auto& [name, perf] : plotLines)
		{
			perf.isActive = false;
			if (perf.elapsedMs.size() != refreshRate)
			{
				perf.elapsedMs.resize(refreshRate);
				perf.index = 0;
			}
		}
	}

	void begin(const char* label)
	{
		const bool init{ plotLines.contains(label) };
		auto* const perf{ &plotLines[label] };

		if (!init)
		{
			perf->isActive = false;
			perf->index = 0;
			perf->elapsedMs.resize(Debug::Performance::refreshRate());
		}

		perf->chrono.start();
	}

	void end(const char* label)
	{
		auto* const perf{ &plotLines.at(label) };
		const auto end{ perf->chrono.elapsedMs() };
		perf->isActive = true;

		if (perf->index >= perf->elapsedMs.size())
		{
			perf->index = 0;
			perf->elapsedMs[perf->index] = end;
		}
		else
		{
			perf->elapsedMs[perf->index++] = end;
		}
	}
}
#endif