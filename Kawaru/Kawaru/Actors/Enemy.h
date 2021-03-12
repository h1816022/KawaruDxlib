#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "Character.h"

class NavMeshMoveComponent;

class Enemy :
    public Character
{
public:
    Enemy(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Enemy();

    void Update(const Input& input)override final;

    void Draw()override final;

protected:
    VECTOR floatingOffset_;

private:
    void UpdatePaths();

    //std::vector<VECTOR> paths_;

    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;
};