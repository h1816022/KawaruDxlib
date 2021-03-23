#include <cassert>
#include "Goal.h"
#include "../Actor.h"
#include "../../Scenes/Scene.h"
#include "../../Systems/Input.h"
#include "../../Application.h"
#include "../../Systems/File.h"
#include "../../Systems/FileManager.h"
#include "../Players/Player.h"

Goal::Goal(Scene& scene, float px, float py, float pz):
	Gimmick(scene, L"Resources/Models/Goal.mqo", L"Player", true, px, py, pz)
{
	suuSE_ = FileManager::Instance().Load(L"Resources/Sounds/Suu.wav")->GetHandle();
	clearSE_ = FileManager::Instance().Load(L"Resources/Sounds/Clear.mp3")->GetHandle();
}

Goal::~Goal()
{
}

void Goal::Update(const Input& input)
{
	Gimmick::Update(input);

	MV1SetRotationXYZ(modelHandle_, VGet(0.0f, angle_, 0.0f));

	angle_ += 0.03f;
	angle_ = (angle_ > DX_PI_F * 2) ? (angle_ - DX_PI_F * 2) : angle_;
}

void Goal::Draw()
{
	MV1DrawModel(modelHandle_);
}

bool Goal::Hit(std::shared_ptr<Actor> hitActor)
{
	if (Application::Instance().SetIsGameClear())
	{
		PlaySoundMem(suuSE_, DX_PLAYTYPE_BACK);

		FileManager::Instance().DelayPlaySound(clearSE_, 80);

		auto player = std::dynamic_pointer_cast<Player>(hitActor);
		if (player == nullptr)
		{
			assert(false);
		}
		else
		{
			player->EndGame();
		}

		return true;
	}

	return false;
}
