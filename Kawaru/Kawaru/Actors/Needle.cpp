#include "Needle.h"
#include "../Scenes/GameplayingScene.h"

Needle::Needle(GameplayingScene& scene):
	Gimmick(scene, L"Models/Needle.mqo", L"Player")
{
	pos_ = VGet(0.0f, 0.0f, 2000.0f);
	MV1SetPosition(modelHandle_, pos_);
}

Needle::~Needle()
{
}

void Needle::Update(const Input& input)
{
	Gimmick::Update(input);
}

void Needle::Draw()
{
	MV1DrawModel(modelHandle_);
}

void Needle::Hit(std::shared_ptr<Actor> hitActor)
{
	hitActor->Destroy();
}