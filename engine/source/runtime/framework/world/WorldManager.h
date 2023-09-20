#pragma once
#include "core/Singleton.h"
#include <filesystem>
#include <string>
#include <vector>
#include "Level.h"
#include <memory>

namespace TinyEngine
{
    struct WorldRes
    {
        std::string              m_name;
        std::vector<std::string> m_levels;
    };
    class WorldManager : public Singleton<WorldManager>
    {
    public:
        WorldManager() = default;
        ~WorldManager() = default;

        void Initialize();
        void Tick(float deltaTime);
        void Clear();

        Level* GetCurrentActiveLevel() const;

        void ReloadCurrentLevel();
        void SaveCurrentLevel();
    private:
        void loadDefaultLevel(); 
        void processPendingLoadWorld();
        void loadWorld(const WorldRes& pending_load_world);
        void loadLevel(const std::string& level_url);

        std::filesystem::path m_pending_load_world_path;
        std::string           m_current_world_name;
        
        std::vector<Level*> m_levels;
        Level*              m_current_active_level {nullptr};

    };
}