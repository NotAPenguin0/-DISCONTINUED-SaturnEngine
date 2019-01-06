#include "FPSCounter.hpp"
#include "Time.hpp"

#include <algorithm>

float FPSCounter::fps()
{
	//1000 because there are 1000 milliseconds in a second
	return 1000.0f / frameTime().count();
}

std::chrono::milliseconds FPSCounter::frameTime()
{ 
	using Saturn::Time;
	return std::chrono::milliseconds{std::max(static_cast<unsigned long long>(Time::deltaTime), 1ull)}; 
}