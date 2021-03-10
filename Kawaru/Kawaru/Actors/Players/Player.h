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

    // �g�[���X���s���ۂɏI�[�Ƃ���T���v�����O���W�́A���g�̍��W����̃I�t�Z�b�g�l
    std::vector<VECTOR> lineTraceSamplingOffsets_;
};