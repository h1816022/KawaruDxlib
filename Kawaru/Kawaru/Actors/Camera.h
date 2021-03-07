#pragma once
#include <memory>
#include "Actor.h"

class Stage;

class Camera :
    public Actor
{
public:
    Camera(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Camera();

    void Update(const Input& input)override final;

    const VECTOR& GetTargetPos()const;

    void SetTargetActor(std::shared_ptr<Actor> target);

private:
    void UpdateAngle(const Input& input);

    void UpdatePos();

    void ClampAngle();

    VECTOR targetPos_;

    // �����p�x
    float angleH_;

    // �����p�x
    float angleV_;

    std::shared_ptr<Actor> targetActor_;

    const Stage& stage_;

    VECTOR setEye_;
    VECTOR setTarget_;
};