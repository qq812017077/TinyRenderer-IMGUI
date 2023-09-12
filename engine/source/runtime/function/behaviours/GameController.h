#pragma once
#include "behaviours/Behaviour.h"
#include "ui/GUI.h"
#include "components/Light.h"
#include "input/Input.h"
class GameController : public Behaviour
{
public:
    using Behaviour::Behaviour;

    void OnGUI() override
    {
        GUI::Begin("Game Controller");
        // visibility
        GUI::BeginGroup();
        for (auto& pGo : GameObject::GetGameObjects())
        {
            auto & name = pGo->GetName();
            auto isActived = pGo->IsActived();
            if(GUI::Checkbox(name.c_str(), &isActived))
            {
                pGo->SetActive(isActived);
            }
        }
        GUI::EndGroup();
        GUI::End();
    }
private:

};