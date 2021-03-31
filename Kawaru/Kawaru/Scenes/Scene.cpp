#include <DxLib.h>
#include <algorithm>
#include "Scene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "SceneController.h"
#include "../Actors/Actor.h"

void Scene::AddActors(std::shared_ptr<Actor> actor)
{
	actors_.emplace_back(actor);
}

std::vector<std::shared_ptr<Actor>> Scene::GetActors(const std::wstring& tag)
{
	std::vector<std::shared_ptr<Actor>> ret;

	for (auto actor : actors_)
	{
		if (actor->GetTag() == tag)
		{
			ret.emplace_back(actor);
		}
	}

	return ret;
}

Scene::Scene(SceneController& controller) :
	controller_(controller)
{
	nowFadeCount_ = 0;
	fadeMode_ = FADE_MODE::Non;
}

void Scene::StartFade(const FADE_MODE& mode, int time)
{
	nowFadeCount_ = 0;
	fadeMode_ = mode;

	fadeInterval_ = time;
}

void Scene::PostDraw()
{
	if (fadeMode_ != FADE_MODE::Non)
	{
		if (fadeMode_ == FADE_MODE::In)
		{
			Fade(1.0f - (nowFadeCount_ / static_cast<float>(fadeInterval_)));
		}
		else
		{
			Fade(nowFadeCount_ / static_cast<float>(fadeInterval_));
		}
	}
	else
	{
		if (nowFadeRate != 0.0f)
		{
			Fade(nowFadeRate);
		}
	}

	++nowFadeCount_;
}

void Scene::DrawDropShadowString(const Position2& pos, const wchar_t* text, unsigned int color)
{
	constexpr int SHIFT_AMOUNT = 2;
	DrawString(pos.x + SHIFT_AMOUNT, pos.y + SHIFT_AMOUNT, text, 0x777777);
	DrawString(pos.x, pos.y, text, color);
}

void Scene::UpdateActors(const Input& input)
{
	for (auto& actor : actors_)
	{
		actor->Update(input);
	}

	auto itr = std::remove_if(actors_.begin(), actors_.end(), [](std::shared_ptr<Actor> actor)
		{
			return actor->CheckIsDead();
		});

	actors_.erase(itr, actors_.end());
}

void Scene::DrawActors()
{
	for (auto& actor : actors_)
	{
		actor->Draw();
	}
}

void Scene::Fade(float rate)
{
	constexpr int BLEND_PARAM_MAX = 255;

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(static_cast<float>(BLEND_PARAM_MAX) * rate));
	DrawBox(0, 0, vpSize.w, vpSize.h, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	nowFadeRate = rate;
}

Scene::~Scene()
{
}

void Scene::UpdateFade(const Input& input)
{
	if (fadeMode_ != FADE_MODE::Non)
	{
		if (nowFadeCount_ > fadeInterval_)
		{
			if (fadeMode_ == FADE_MODE::In)
			{
				EndFadeIn();
			}
			else
			{
				EndFadeOut();
			}
			fadeMode_ = FADE_MODE::Non;
		}
	}
}