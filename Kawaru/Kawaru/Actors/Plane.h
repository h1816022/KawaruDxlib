#pragma once
#include "Actor.h"
class Plane :
    public Actor
{
public:
    Plane(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Plane();

    void Update(const Input& input)override final;
    void Draw()override final;
};