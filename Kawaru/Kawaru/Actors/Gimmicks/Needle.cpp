#include "Needle.h"
#include "../../Scenes/Scene.h"
#include "../../Systems/File.h"
#include "../../Systems/FileManager.h"

namespace
{
	// モデルの高さサイズ
	constexpr float MODEL_HEIGHT = 300.0f;

	// 動くまでの待ち時間
	constexpr int MOVE_INTERVAL = 120;

	// 動くのに掛かる時間
	constexpr int MOVE_TIME = 20;
}

Needle::Needle(Scene& scene, float px, float py, float pz):
	Gimmick(scene, L"Resources/Models/Needle.mqo", L"Player", true, px, py, pz)
{
	hitSE_ = FileManager::Instance().Load(L"Resources/Sounds/HitNeedle.mp3")->GetHandle();

	moveLengthPerFrame_ = -MODEL_HEIGHT / static_cast<float>(MOVE_TIME);
}

Needle::~Needle()
{
}

void Needle::Update(const Input& input)
{
	Gimmick::Update(input);

	if (moveCount_ > MOVE_INTERVAL)
	{
		pos_ = VAdd(pos_, VGet(0.0f, moveLengthPerFrame_, 0.0f));
		MV1SetPosition(modelHandle_, pos_);
	}

	++moveCount_;
	
	if (moveCount_ > MOVE_INTERVAL + MOVE_TIME)
	{
		moveCount_ = 0;

		moveLengthPerFrame_ *= -1.0f;

		collisionEnabled_ = !collisionEnabled_;
	}
}

void Needle::Draw()
{
	MV1DrawModel(modelHandle_);
}

bool Needle::Hit(std::shared_ptr<Actor> hitActor)
{
	if (hitActor->Destroy())
	{
		PlaySoundMem(hitSE_, DX_PLAYTYPE_BACK);
		return true;
	}

	return false;
}