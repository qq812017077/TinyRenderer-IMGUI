#include "WorldManager.h"

#include <unordered_set>
#include "managers/RenderQueueManager.h"
#include "object/GameObject.h"
#include "scene/SceneManager.h"
#include "Level.h"

namespace TinyEngine
{
    void WorldManager::Initialize()
    {
        // m_pending_load_world_path = "res/worlds/test.world";
    }
    void WorldManager::Tick(float deltaTime)
    {
        processPendingLoadWorld();
        if (m_current_active_level == nullptr) loadDefaultLevel();
        
        RenderQueueManager::Clear();
        // SceneManager::Get().Clear();
        for (Level* level : m_levels)
        {
            level->Tick(deltaTime);
        }
    }
    
    void WorldManager::Clear()
    {
        m_pending_load_world_path.clear();
        m_current_world_name.clear();

        m_current_active_level = nullptr;
        for (Level* level : m_levels)
        {
            level->Clear();
            delete level;
        }
        m_levels.clear();
    }

    Level* WorldManager::GetCurrentActiveLevel() const { return m_current_active_level; }

    void WorldManager::ReloadCurrentLevel()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }
    void WorldManager::SaveCurrentLevel()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void WorldManager::loadDefaultLevel()
    {
        m_current_active_level = new Level();
        m_levels.push_back(m_current_active_level);
        m_current_active_level->LoadDefault();
    }

    void WorldManager::processPendingLoadWorld()
    {
        if (m_pending_load_world_path.empty())
            return;

        std::string pending_load_world_path = m_pending_load_world_path.generic_string();

        WorldRes world_res;
        // AssetManager::getInstance().loadAsset(m_pending_load_world_path, world_res);

        m_pending_load_world_path.clear();
        if (world_res.m_name == m_current_world_name)
            return;

        Clear();

        loadWorld(world_res);
    }
    void WorldManager::loadWorld(const WorldRes& pending_load_world)
    {
        m_current_world_name = pending_load_world.m_name;

        typedef std::unordered_set<size_t> TypeIDSet;
        for (const std::string& level_url : pending_load_world.m_levels)
        {
            loadLevel(level_url);
        }
    }
    void WorldManager::loadLevel(const std::string& level_url)
    {
        Level* level = new Level();
        level->Load(level_url);
        m_levels.push_back(level);

        if (m_current_active_level == nullptr)
        {
            m_current_active_level = level;
        }
    }
} // namespace TinyEngine