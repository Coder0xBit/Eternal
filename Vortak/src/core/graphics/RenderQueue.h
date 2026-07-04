#pragma once

#include "utils/Base.h"

namespace Vortak {
    template<typename Command>
    class RenderQueue {
    public :
        RenderQueue() = default;
        RenderQueue(const RenderQueue& other) = delete;
        RenderQueue(RenderQueue&& other) = delete;

        RenderQueue& operator=(const RenderQueue& other) = delete;
        RenderQueue& operator=(RenderQueue&& other) = delete;

        void push(Command command) {
            std::lock_guard<std::mutex> lock(mMutex);
            mCommands.push(std::move(command));
            mCond.notify_one();
        }

        Command waitAndPop() {
            std::unique_lock<std::mutex> lock(mMutex);
            mCond.wait(lock, [this]() { return mCommands.empty(); });
            Command command = mCommands.front();
            mCommands.pop();
            return command;
        }

    private :
        std::queue<Command> mCommands;
        std::queue<Command> mRunningCommands;
        std::mutex mMutex;
        std::condition_variable mCond;
    };
}
