#include <cassert>
#include "Candle.h"
#include "../../Systems/File.h"
#include "../../Systems/FileManager.h"
#include "../Players/Player.h"

Candle::Candle(Scene& scene, float px, float py, float pz):
	Gimmick(scene, L"Resources/Models/Candle.mqo", L"Player", true, px, py, pz)
{
	hitSE_ = FileManager::Instance().Load(L"Resources/Sounds/HitNeedle.mp3")->GetHandle();

	MV1SetScale(modelHandle_, VGet(0.4f, 0.4f, 0.4f));
}

Candle::~Candle()
{
}

void Candle::Update(const Input& input)
{
	Gimmick::Update(input);

	MV1SetRotationXYZ(modelHandle_, VGet(0.0f, angle_, 0.0f));

	angle_ += 0.03f;
	angle_ = (angle_ > DX_PI_F * 2) ? (angle_ - DX_PI_F * 2) : angle_;
}

void Candle::Draw()
{
	MV1DrawModel(modelHandle_);
}

bool Candle::Hit(std::shared_ptr<Actor> hitActor)
{
	PlaySoundMem(hitSE_, DX_PLAYTYPE_BACK);

	auto player = std::dynamic_pointer_cast<Player>(hitActor);
	if (player == nullptr)
	{
		assert(false);
	}
	else
	{
		player->AddCallNum();
	}

	Destroy();

	return true;
}
