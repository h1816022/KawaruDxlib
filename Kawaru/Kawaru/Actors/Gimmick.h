#pragma once
#include <memory>
#include "Actor.h"

class Scene;

class Gimmick :
    public Actor
{
public:
    Gimmick(Scene& scene, const wchar_t* modelFilePath, const wchar_t* targetTag);
    virtual ~Gimmick();

    virtual void Update(const Input& input)override;
    virtual void Draw() = 0;

protected:
    virtual void Hit(std::shared_ptr<Actor> hitActor) = 0;

    const std::wstring targetTag_;

    Scene& scene_;
};

