#include <cassert>
#include "SceneController.h"
#include "Scene.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "GameplayingScene.h"

SceneController::SceneController()
{
	scenes_.emplace_front(new GameplayingScene(*this));
}

void SceneController::ChangeScene(Scene* newScene)
{
	postDrawExecuter_.emplace_back([this, newScene]()
		{
			scenes_.pop_front();

			scenes_.emplace_front(newScene);
		});
}

void SceneController::PushScene(Scene* newScene)
{
	postDrawExecuter_.emplace_back([this, newScene]()
		{
			scenes_.emplace_front(newScene);
		});
}

void SceneController::PopScene(unsigned int num)
{
	if (scenes_.empty())
	{
		return;
	}

	unsigned int n = num;

	if (scenes_.size() < num)
	{
		n = static_cast<unsigned int>(scenes_.size());
	}

	for (; n > 0; --n)
	{
		postDrawExecuter_.emplace_back([this]()
			{
				scenes_.erase(scenes_.begin());
			});
	}
}

void SceneController::CleanChangeScene(Scene* newScene)
{
	postDrawExecuter_.emplace_back([this, newScene]()
		{
			scenes_.clear();

			scenes_.emplace_front(newScene);
		});
}

void SceneController::ClearScenes()
{
	scenes_.clear();
}

void SceneController::Update(const Input& input)
{
	assert(!scenes_.empty());

	scenes_.front()->Update(input);
}

void SceneController::Draw()
{
	assert(!scenes_.empty());

	for (auto rit = scenes_.rbegin(); rit != scenes_.rend(); ++rit)
	{
		(*rit)->Draw();
	}

	for (auto& p : postDrawExecuter_)
	{
		p();
	}

	postDrawExecuter_.clear();
}

std::shared_ptr<Scene> SceneController::GetSceneFromTop(int num)
{
	if (scenes_.size() < num)
	{
		return nullptr;
	}

	return scenes_[scenes_.size() - num];
}
