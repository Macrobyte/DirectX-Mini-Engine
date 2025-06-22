#include "Time.h"

std::chrono::time_point<std::chrono::high_resolution_clock> Time::s_startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> Time::s_previousTime;
float Time::deltaTime = 0;
float Time::timeScale = 0;

void Time::Initialize() {
	s_startTime = std::chrono::high_resolution_clock::now();
	s_previousTime = s_startTime;
	timeScale = 1.0f;
}

void Time::Update()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - s_previousTime).count() / 1000000.0f;
	deltaTime *= timeScale;
	s_previousTime = currentTime;
}

float Time::DeltaTime()
{
	return deltaTime;
}

