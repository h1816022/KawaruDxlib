#pragma once
#include <string>
#include <vector>
#include "../Character.h"

class Camera;

class Player :
    public Character
{
public:
    Player(const Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Player();

    void Update(const Input& input)override final;

    void Draw()override final;

    const std::vector<VECTOR>& GetLineTraceSamplingOffsets()const;

private:
    void IdleUpdate(const Input& input)override final;
    void RunUpdate(const Input& input)override final;

    void CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec);

    bool CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input);

    const Camera& camera_;

    // トーレスを行う際に終端とするサンプリング座標の、自身の座標からのオフセット値
    std::vector<VECTOR> lineTraceSamplingOffsets_;
};