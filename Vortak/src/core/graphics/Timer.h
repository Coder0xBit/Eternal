#pragma once

#include <chrono>

namespace Vortak {
    class Timestep {
    public:
        Timestep(double time = 0.0f)
            : mTime(time) {
        }

        operator double() const { return mTime; }

        double seconds() const { return mTime; }
        double milliSeconds() const { return mTime * 1000.0f; }

    private:
        double mTime;
    };

    class Timer {
    public:
        Timer() = default;

        ~Timer() = default;

        void start() {
            mLastFrameTime = std::chrono::high_resolution_clock::now();
        }

        Timestep tick() {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> delta = now - mLastFrameTime;
            mLastFrameTime = now;
            Timestep timeStep(delta.count());
            return timeStep;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
    };
}
