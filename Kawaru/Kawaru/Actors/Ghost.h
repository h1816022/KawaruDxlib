#pragma once
#include <memory>
#include "Character.h"

class Input;
class Stage;
class NavMeshMoveComponent;
class Camera;

class Ghost :
    public Character
{
public:
    Ghost(Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Ghost();

    void Update(const Input& input)override final;

    void Draw();

private:
    bool ApproachPlayer();

    void ApproachTarget(const VECTOR& target);

    VECTOR CalcApproachPos(const VECTOR& target);

    void EndMove();

    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;

    VECTOR floatingOffset_;

    Camera& camera_;

    bool followingPlayer_ = false;
};