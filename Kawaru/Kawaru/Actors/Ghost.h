#pragma once
#include <memory>
#include "Character.h"

class Input;
class Stage;
class NavMeshMoveComponent;

class Ghost :
    public Character
{
public:
    Ghost(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Ghost();

    void Update(const Input& input)override final;

    void Draw();

private:
    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;

    VECTOR floatingOffset_;
};