#pragma once

class ShaderManager
{
public:
    static ShaderManager& GetInstance();

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager(ShaderManager&&) = delete;
    ShaderManager& operator=(ShaderManager&&) = delete;

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

public:
    void Initialize();

    std::vector<char> GetShader(const std::string& shader_name, const std::string& target);

private:
    void LoadShader(const std::string& file_path);
    void SetShader(const std::string& shader_name, const std::vector<char>& shader);

private:
    std::unordered_map<std::string, std::vector<char>> shaders_;
};

