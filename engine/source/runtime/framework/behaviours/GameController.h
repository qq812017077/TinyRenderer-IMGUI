#pragma once
#include "behaviours/Behaviour.h"
#include "ui/GUI.h"
#include "components/Light.h"
#include "input/Input.h"
class GameController : public Behaviour
{
public:
    using Behaviour::Behaviour;
    void Init() override
    {
        modelGO = GameObject::CreateFromFile("res/models/spider.fbx");
        if(modelGO)
        {
            modelPos = { 3.0f, 0.0f, 0.0f };
            modelGO->transform.SetPosition(modelPos);
            modelRot = { 0.0f, 0.0f, 0.0f };
            modelGO->transform.SetEulerAngle(modelRot);
            modelGO->transform.SetScale({ 0.01f, 0.01f, 0.01f });
        }
        selectedGO = modelGO;
    }

    void OnUpdate(float deltaTime) override
    {
        modelGO->transform.SetPosition(modelPos);
        modelGO->transform.SetEulerAngle(modelRot);
    }
    void OnGUI() override
    {
        // if(GUI::Begin("GameObjects"))
        // {
        //     GUI::Columns(2, nullptr, true);

        //     GUI::BeginChild("hierarchy", { 0, 150 }, true);
        //     // render hierarchy
        //     for (auto& pGo : GameObject::GetRootGameObjects())
        //     {
        //         auto & name = pGo->GetName();
        //         GUI::Text(name.c_str());
        //     }
        //     GUI::EndChild();

        //     GUI::NextColumn();

        //     if(selectedGO != nullptr)
        //     {
        //         auto & name = selectedGO->GetName();
        //         auto isActived = selectedGO->IsActived();
        //         if (GUI::Checkbox(name.c_str(), &isActived))
        //         {
        //             selectedGO->SetActive(isActived);
        //         }
        //         GUI::DragFloat3("Position", reinterpret_cast<float*>(&modelPos), 0.1f);
        //         GUI::DragFloat3("Rotation", reinterpret_cast<float*>(&modelRot), 0.1f);
        //     }
        // }
        // GUI::End();
    }
private:
    GameObject * modelGO;
    GameObject * parentGO;
    GameObject * childGO;
    Vector3 modelPos;
    Vector3 modelRot;
    GameObject * selectedGO;
};