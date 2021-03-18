#pragma once
#include "Gimmick.h"

class GameplayingScene;

class Needle :
    public Gimmick
{
public:
    Needle(GameplayingScene& scene);
    ~Needle();

    void Update(const Input& input)override final;
    void Draw()override final;

protected:
    void Hit(std::shared_ptr<Actor> hitActor)override final;

private:
    int hitSE_;
};