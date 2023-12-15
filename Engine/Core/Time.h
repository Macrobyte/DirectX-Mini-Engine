#pragma once
#include <chrono>

class Time {
public:
	static void Initialize();

	static void Update();

	static float DeltaTime();

	static void SetTimeScale(float scale) { timeScale = scale; }
	static float GetTimeScale() { return timeScale; }

	private:
	static std::chrono::time_point<std::chrono::high_resolution_clock> s_startTime;
	static std::chrono::time_point<std::chrono::high_resolution_clock> s_previousTime;
	static float deltaTime;
	static float timeScale;

};

