#pragma once
#include "Actor.h"
class Cube :
    public Actor
{
public:
    Cube(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f, 
        const float scaleX = 1.0f, const float scaleY = 1.0f, const float scaleZ = 1.0f);
    ~Cube();

    void Update(const Input& input)override final;
    void Draw()override final;

private:
    VECTOR scale_;
};