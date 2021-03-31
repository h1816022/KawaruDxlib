#include <DxLib.h>
#include <stdint.h>
#include "TitleScene.h"
#include "SceneController.h"
#include "GameplayingScene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "../Tools/Geometry.h"
#include "../Systems/FileManager.h"
#include "../Systems/File.h"

namespace
{
	// 遷移させようとしてからフェードが始まるまでの遅れる時間
	constexpr uint32_t FADE_DELAY = 40;

	// 基本の点滅のインターバル
	constexpr uint32_t BLINK_INTERVAL_DEFAULT = 60;

	// 早くするときの点滅のインターバル
	constexpr uint32_t BLINK_INTERVAL_FAST = 3;

	// 点滅で消えてから点くまでの時間
	constexpr uint32_t BLINK_TIME = 3;
}

TitleScene::TitleScene(SceneController& controller) :
	Scene(controller)
{
	updater_ = &TitleScene::NormalUpdate;
	drawer_ = &TitleScene::FadeInDraw;

	blinkCount_ = 0;

	auto& fileManager = FileManager::Instance();

	titleImage_ = fileManager.Load(L"Resources/Images/TitleLogo.png")->GetHandle();
	pressStartImage_ = fileManager.Load(L"Resources/Images/PressStart.png")->GetHandle();

	okSE_ = fileManager.Load(L"Resources/Sounds/OK.mp3")->GetHandle();

	StartFade(FADE_MODE::In);
}

void TitleScene::NormalUpdate(const Input& input)
{
	if (input.IsTriggered("OK"))
	{
		StartFade(FADE_MODE::Out);

		updater_ = &TitleScene::FadeoutUpdate;
		drawer_ = &TitleScene::FadeOutDraw;

		PlaySoundMem(okSE_, DX_PLAYTYPE_BACK);

		StartJoypadVibration(DX_INPUT_PAD1, 500, 100);

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
	auto wSize = Application::Instance().GetViewport().GetSize();

	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (blinkCount_ < BLINK_INTERVAL_DEFAULT)
	{
		DrawRotaGraph(wSize.w / 2.0f, wSize.h / 7.0f * 5.0f, 0.7, 0.0, pressStartImage_, true);
	}

	if (blinkCount_ > BLINK_INTERVAL_DEFAULT + BLINK_TIME)
	{
		blinkCount_ = 0;
	}
}

void TitleScene::FadeInDraw()
{
	auto wSize = Application::Instance().GetViewport().GetSize();
	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::FadeOutDraw()
{
	auto wSize = Application::Instance().GetViewport().GetSize();
	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	DrawRotaGraph(wSize.w / 2.0f, wSize.h / 3.0f, 1.0, 0.0, titleImage_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	if (blinkCount_ < BLINK_INTERVAL_FAST)
	{
		DrawRotaGraph(wSize.w / 2.0f, wSize.h / 7.0f * 5.0f, 0.7, 0.0, pressStartImage_, true);
	}

	if (blinkCount_ > BLINK_INTERVAL_FAST + BLINK_TIME)
	{
		blinkCount_ = 0;
	}
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(const Input& input)
{
	blinkCount_++;
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
