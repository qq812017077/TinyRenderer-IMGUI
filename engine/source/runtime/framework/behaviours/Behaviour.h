#pragma once
#include "components/Component.h"
#include "object/GameObject.h"
#include "components/Renderer.h"
#include "geometry/Primitive.h"
#include "Color.h"
class Behaviour : public Component
{
public:
    using Component::Component;
protected:
    void Init() override {}
};