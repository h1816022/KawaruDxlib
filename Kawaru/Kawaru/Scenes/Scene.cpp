#include <DxLib.h>
#include <algorithm>
#include "Scene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "../Geometry.h"
#include "SceneController.h"
#include "../Actors/Actor.h"

void Scene::AddActors(std::shared_ptr<Actor> actor)
{
	actors_.emplace_back(actor);
}

Scene::Scene(SceneController& controller) :
	controller_(controller)
{
	fadeCount_ = 0;
	fadeMode_ = FADE_MODE::Non;
}

void Scene::StartFade(const FADE_MODE& mode)
{
	fadeCount_ = 0;
	fadeMode_ = mode;
}

void Scene::PostDraw()
{
	if (fadeMode_ != FADE_MODE::Non)
	{
		if (fadeMode_ == FADE_MODE::In)
		{
			Fade(1.0f - (fadeCount_ / static_cast<float>(FADE_INTERVAL)));
		}
		else
		{
			Fade(fadeCount_ / static_cast<float>(FADE_INTERVAL));
		}
	}
	else
	{
		if (nowFadeRate != 0.0f)
		{
			Fade(nowFadeRate);
		}
	}

	++fadeCount_;
}

void Scene::DrawDropShadowString(const Position2& pos, const wchar_t* text, unsigned int color)
{
	DrawString(pos.x + 2, pos.y + 2, text, 0x777777);
	DrawString(pos.x, pos.y, text, color);
}

void Scene::UpdateActors(const Input& input)
{
	for (auto& actor : actors_)
	{
		actor->Update(input);
	}
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
	constexpr int blendParamMax = 255;

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(static_cast<float>(blendParamMax) * rate));
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
		if (fadeCount_ > FADE_INTERVAL)
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