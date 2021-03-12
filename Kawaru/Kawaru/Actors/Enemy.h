#pragma once
#include "Character.h"
#include "../NavMeshPath.h"

class Enemy :
    public Character
{
public:
    Enemy(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Enemy();

    void Update(const Input& input)override final;

    void Draw()override final;

private:
    bool CheckPath()const;

    bool ReachToGoal()const;

    void UpdatePaths();

    VECTOR goalPos[3];

    std::vector<VECTOR> paths_;
};