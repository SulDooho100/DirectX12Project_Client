#pragma once

class SceneManager
{ 
public:
    static SceneManager& GetInstance();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

private:
    SceneManager() = default;
    ~SceneManager() = default;

public:
    void Initialize();
    void Draw() const;

private: 

private: 
};

