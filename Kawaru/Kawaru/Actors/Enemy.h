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
    Enemy(Scene& scene, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Enemy();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw()override final;

private:
    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;
};