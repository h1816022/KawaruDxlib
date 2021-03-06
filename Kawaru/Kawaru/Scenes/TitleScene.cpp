#include <DxLib.h>
#include <stdint.h>
#include "TitleScene.h"
#include "SceneController.h"
#include "GameplayingScene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "../Geometry.h"
//#include "../Systems/FileManager.h"
//#include "../Systems/File.h"

namespace
{
	// 遷移させようとしてからフェードが始まるまでの遅れる時間
	constexpr uint32_t FADE_DELAY = 40;

	// 基本の点滅のインターバル
	constexpr uint32_t BLINK_INTERVAL_DEFAULT = 120;

	// 早くするときの点滅のインターバル
	constexpr uint32_t BLINK_INTERVAL_FAST = 10;

	// 点滅で消えてから点くまでの時間
	constexpr uint32_t BLINK_TIME = 5;

	constexpr uint32_t EMISSIVE_TIME = 30;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller)
{
	updater_ = &TitleScene::NormalUpdate;
	drawer_ = &TitleScene::FadeInDraw;

	count_ = 0;

	StartFade(FadeMode::In);
}

void TitleScene::NormalUpdate(const Input& input)
{
	if (input.IsTriggered("OK"))
	{
		controller_.ChangeScene(new GameplayingScene(controller_));
		return;
	}
}

void TitleScene::FadeinUpdate(const Input&)
{
}

void TitleScene::FadeoutUpdate(const Input&)
{
}

void TitleScene::NormalDraw()
{
	DrawString(0, 0, L"Title", 0xffffffff);
}

void TitleScene::FadeInDraw()
{

}

void TitleScene::FadeOutDraw()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(const Input& input)
{
	count_++;
	(this->*updater_)(input);
	Scene::UpdateFade(input);
}

void TitleScene::Draw()
{
	(this->*drawer_)();

	Scene::PostDraw();
}

void TitleScene::EndFadeIn()
{
	updater_ = &TitleScene::NormalUpdate;
	drawer_ = &TitleScene::NormalDraw;
}

void TitleScene::EndFadeOut()
{
	controller_.ChangeScene(new GameplayingScene(controller_));
}
