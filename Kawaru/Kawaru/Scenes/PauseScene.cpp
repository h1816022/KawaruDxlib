#include <DxLib.h>
#include <algorithm>
#include "PauseScene.h"
#include "../Systems/Input.h"
#include "SceneController.h"
#include "../Application.h"
#include "../Geometry.h"
#include "TitleScene.h"
#include "../Tools/Arithmetic.h"

PauseScene::PauseScene(SceneController& controller) :
	MenuSceneBase(controller)
{
	title_ = L"Pause Menu";

	InitMenuItems(L"ゲームに戻る", [this]()
		{
			CloseMenu();
		});

	InitMenuItems(L"タイトルに戻る", [this]()
		{
			ReturnToTitle();
		});

	InitMenuItems(L"ゲームを終了する", [this]()
		{
			Application::Instance().Exit();
		});

	InitMenu();

	Scene::StartFade(FADE_MODE::In);
}

PauseScene::~PauseScene()
{
}

void PauseScene::Update(const Input& input)
{
	MenuSceneBase::Update(input);

	if (delayFunctions_.size() != 0)
	{
		for (auto& f : delayFunctions_)
		{
			++f.nowCount;
		}

		auto it = remove_if(delayFunctions_.begin(), delayFunctions_.end(), [&](DelayFuncData data)
			{
				if (data.nowCount >= data.endCount)
				{
					(this->*(data.func))();
				}
				return false;
			});

		delayFunctions_.erase(it, delayFunctions_.end());
	}
}

void PauseScene::Draw()
{
	MenuSceneBase::Draw();
}

void PauseScene::ReturnToTitle()
{
	controller_.CleanChangeScene(new TitleScene(controller_));
}