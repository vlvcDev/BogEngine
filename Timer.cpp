#include "Timer.h"

Timer::Timer() {
    Reset();
}

void Timer::Reset() {
    lastTime = std::chrono::steady_clock::now();
}

float Timer::GetElapsedTime() const {
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - lastTime;
    return elapsedTime.count();
}
