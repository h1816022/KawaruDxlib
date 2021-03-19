#include "Goal.h"
#include "Actor.h"
#include "../Scenes/Scene.h"
#include "../Systems/Input.h"

Goal::Goal(Scene& scene):
	Gimmick(scene, L"Models/Goal.mqo", L"Player")
{

}

Goal::~Goal()
{
}

void Goal::Update(const Input& input)
{
}

void Goal::Draw()
{
}

void Goal::Hit(std::shared_ptr<Actor> hitActor)
{
}
