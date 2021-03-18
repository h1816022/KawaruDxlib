#include <DxLib.h>
#include "GameplayingScene.h"
#include "SceneController.h"
#include "ResultScene.h"
#include "../Application.h"
#include "../Systems/Input.h"
#include "../Actors/Players/Player.h"
#include "../Actors/Camera.h"
#include "../Actors/Stage.h"
#include "../NavMesh/NavMesh.h"
#include "../Actors/Enemy.h"
#include "../Actors/Ghost.h"
#include "../Actors/Needle.h"
#include "PauseScene.h"

GameplayingScene::GameplayingScene(SceneController& controller) :
	Scene(controller),
	updater_(&GameplayingScene::FadeInUpdate),
	drawer_(&GameplayingScene::FadeInDraw),
	vpSize_(Application::Instance().GetViewport().GetSize())
{
	SetFontSize(15);

	StartFade(FADE_MODE::In);

	auto navMesh = std::make_shared<NavMesh>(*this);
	AddActors(navMesh);

	auto stage = std::make_shared<Stage>(*this, *navMesh);
	AddActors(stage);

	auto needle = std::make_shared<Needle>(*this);
	AddActors(needle);

	auto camera = std::make_shared<Camera>(*this, *stage);
	AddActors(camera);

	auto ghost = std::make_shared<Ghost>(*this, *camera, *stage, -2000.0f, 500.0f, 0.0f);
	AddActors(ghost);

	auto player = std::make_shared<Player>(*this, *camera, *ghost, *stage, 0.0f, 0.0f, 0.0f);
	AddActors(player);

	camera->SetTargetActor(player);
	camera->SetPlayer(player);
}

void GameplayingScene::NormalUpdate(const Input& input)
{
	const auto& app = Application::Instance();

	if (app.CheckIsGameClear() || app.CheckIsGameOver())
	{
		controller_.ChangeScene(new ResultScene(controller_));
		return;
	}

	if (input.IsTriggered("Pause"))
	{
		controller_.PushScene(new PauseScene(controller_));
		return;
	}

	Scene::UpdateActors(input);
}

void GameplayingScene::FadeInUpdate(const Input& input)
{
	Scene::UpdateActors(input);
}

void GameplayingScene::ChangeSceneUpdate(const Input& input)
{
}

void GameplayingScene::EndFadeIn()
{
	updater_ = &GameplayingScene::NormalUpdate;
	drawer_ = &GameplayingScene::NormalDraw;
	Application::Instance().StartRecording();
}

void GameplayingScene::EndFadeOut()
{
	controller_.ChangeScene(new ResultScene(controller_));
}

void GameplayingScene::NormalDraw()
{
	int nowTime = Application::Instance().GetResultData().playTime;

	DrawFormatString(0, 0, 0xffffff, L"%04d:%02d", nowTime / 60, (nowTime % 60));
}

void GameplayingScene::FadeInDraw()
{
}

GameplayingScene::~GameplayingScene()
{
}

void GameplayingScene::Update(const Input& input)
{
	(this->*updater_)(input);
	Scene::UpdateFade(input);

	++nowCount_;
}

void GameplayingScene::Draw()
{
	Scene::DrawActors();

	(this->*drawer_)();

	Scene::PostDraw();
}