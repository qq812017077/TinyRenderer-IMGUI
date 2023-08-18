#pragma once
#include "core/Singleton.h"

class RendererQueueManager: public Singleton<RendererQueueManager>
{
public:
    RendererQueueManager();
    ~RendererQueueManager();
    RendererQueueManager(RendererQueueManager const&) = delete;
    RendererQueueManager& operator=(RendererQueueManager const&) = delete;

    

private:

};