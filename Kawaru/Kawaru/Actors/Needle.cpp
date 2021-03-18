#include "Needle.h"
#include "../Scenes/GameplayingScene.h"
#include "../File.h"
#include "../FileManager.h"

Needle::Needle(GameplayingScene& scene):
	Gimmick(scene, L"Resources/Models/Needle.mqo", L"Player")
{
	pos_ = VGet(0.0f, 0.0f, 2000.0f);
	MV1SetPosition(modelHandle_, pos_);

	hitSE_ = FileManager::Instance().Load(L"Resources/Sounds/HitNeedle.mp3")->GetHandle();
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
	if (hitActor->Destroy())
	{
		PlaySoundMem(hitSE_, DX_PLAYTYPE_BACK);
	}
}