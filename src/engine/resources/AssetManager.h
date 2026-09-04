#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "renderer/Shader.h"
#include "renderer/Texture.h"

class AssetManager {
public:
    static std::shared_ptr<Shader> GetShader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string key = vertexPath + "|" + fragmentPath;
        if (s_Shaders.find(key) != s_Shaders.end()) {
            return s_Shaders[key];
        }

        auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
        s_Shaders[key] = shader;
        return shader;
    }

    static std::shared_ptr<Texture> GetTexture(const std::string& path) {
        if (s_Textures.find(path) != s_Textures.end()) {
            return s_Textures[path];
        }

        auto texture = std::make_shared<Texture>(path);
        s_Textures[path] = texture;
        return texture;
    }

    static void Clear() {
        s_Shaders.clear();
        s_Textures.clear();
    }

private:
    inline static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;
    inline static std::unordered_map<std::string, std::shared_ptr<Texture>> s_Textures;
};