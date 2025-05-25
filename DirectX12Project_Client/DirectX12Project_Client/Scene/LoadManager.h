#pragma once

class LoadManager
{
public:
    static LoadManager& GetInstance();

    LoadManager(const LoadManager&) = delete;
    LoadManager& operator=(const LoadManager&) = delete;
    LoadManager(LoadManager&&) = delete;
    LoadManager& operator=(LoadManager&&) = delete;

private:
    LoadManager() = default;
    ~LoadManager() = default;

public:
    void Initialize();

private:

private:
};