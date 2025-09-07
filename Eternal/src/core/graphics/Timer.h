#pragma once

#include <chrono>

namespace Eternal {
    class Timestep {
    public:
        Timestep(double time = 0.0f)
            : m_Time(time) {
        }

        operator double() const { return m_Time; }

        double seconds() const { return m_Time; }
        double milliSeconds() const { return m_Time * 1000.0f; }

    private:
        double m_Time;
    };

    class Timer {
    public:
        Timer() = default;

        ~Timer() = default;

        void start() {
            m_LastFrameTime = std::chrono::high_resolution_clock::now();
        }

        Timestep tick() {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> delta = now - m_LastFrameTime;
            m_LastFrameTime = now;
            Timestep timeStep(delta.count());
            return timeStep;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_LastFrameTime;
    };
}
