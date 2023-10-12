#include "Scene.h"

namespace TinyEngine
{
    void Scene::Load()
    {
        m_loaded = true;
    }


    void Scene::Clear()
    {
        m_loaded = false;
        effectDescs.clear();
        selectedRenderers.clear();
    }
}