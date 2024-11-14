#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer();

    void Reset();                   // Resets the timer to the current time
    float GetElapsedTime() const;   // Returns the elapsed time in seconds since the last reset

private:
    std::chrono::steady_clock::time_point lastTime;
};

#endif // TIMER_H
