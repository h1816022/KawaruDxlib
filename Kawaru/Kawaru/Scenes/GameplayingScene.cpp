#include <DxLib.h>
#include "GameplayingScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "../Application.h"
#include "../Systems/Input.h"
#include "../Actors/Players/Player.h"
#include "../Actors/Camera.h"
#include "../Actors/Plane.h"
#include "../Actors/Stage.h"

GameplayingScene::GameplayingScene(SceneController& controller) :
	Scene(controller),
	updater_(&GameplayingScene::NormalUpdate),
	drawer_(&GameplayingScene::FadeInDraw),
	vpSize_(Application::Instance().GetViewport().GetSize())
{
	StartFade(FadeMode::In);

	auto stage = std::make_shared<Stage>();
	AddActors(stage);

	auto camera = std::make_shared<Camera>(*stage, 0.0f, 500.0f, -4000.0f);
	AddActors(camera);

	auto player = std::make_shared<Player>(*camera, *stage, 0.0f, 0.0f, 0.0f);
	AddActors(player);

	camera->SetTargetActor(player);
}

void GameplayingScene::NormalUpdate(const Input& input)
{
	if (input.IsTriggered("OK"))
	{
		controller_.ChangeScene(new TitleScene(controller_));
		return;
	}
}

void GameplayingScene::ChangeSceneUpdate(const Input& input)
{
}

void GameplayingScene::EndFadeIn()
{
	drawer_ = &GameplayingScene::NormalDraw;
}

void GameplayingScene::EndFadeOut()
{
	controller_.ChangeScene(new TitleScene(controller_));
}

void GameplayingScene::NormalDraw()
{
	DrawString(0, 0, L"Game", 0xffffffff);
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

	Scene::UpdateActors(input);

	++nowCount_;
}

void GameplayingScene::Draw()
{
	(this->*drawer_)();

	Scene::DrawActors();

	Scene::PostDraw();
}

void GameplayingScene::StageClear()
{
	Scene::StartFade(FadeMode::Out);
}
