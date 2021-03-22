#include <DxLib.h>
#include <stdint.h>
#include "ResultScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "../Systems/Input.h"
#include "../Application.h"
#include "../Geometry.h"
#include "../FileManager.h"
#include "../File.h"

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

ResultScene::ResultScene(SceneController& controller) :
	Scene(controller)
{
	updater_ = &ResultScene::NormalUpdate;
	drawer_ = &ResultScene::FadeInDraw;

	blinkCount_ = 0;

	auto wSize = Application::Instance().GetViewport().GetSize();

	logoPos = Vector2f(wSize.w / 8.0f, wSize.h / 8.0f);

	auto& fileManager = FileManager::Instance();

	resultImage_ = fileManager.Load(L"Resources/Images/ResultLogo.png")->GetHandle();
	pressStartImage_ = fileManager.Load(L"Resources/Images/PressStart.png")->GetHandle();

	okSE_ = fileManager.Load(L"Resources/Sounds/OK.mp3")->GetHandle();

	SetFontSize(32);

	StartFade(FADE_MODE::In);
}

void ResultScene::NormalUpdate(const Input& input)
{
	if (input.IsTriggered("OK"))
	{
		StartFade(FADE_MODE::Out);

		updater_ = &ResultScene::FadeOutUpdate;
		drawer_ = &ResultScene::FadeOutDraw;

		PlaySoundMem(okSE_, DX_PLAYTYPE_BACK);

		StartJoypadVibration(DX_INPUT_PAD1, 500, 100);

		return;
	}
}

void ResultScene::FadeinUpdate(const Input&)
{
}

void ResultScene::FadeOutUpdate(const Input&)
{
}

void ResultScene::NormalDraw()
{
	DrawGraph(logoPos.x, logoPos.y, resultImage_, true);

	DrawResultElement(BLINK_INTERVAL_DEFAULT);
}

void ResultScene::FadeInDraw()
{
	DrawGraph(logoPos.x, logoPos.y, resultImage_, true);
}

void ResultScene::FadeOutDraw()
{
	DrawGraph(logoPos.x, logoPos.y, resultImage_, true);

	DrawResultElement(BLINK_INTERVAL_FAST);
}

void ResultScene::DrawResultElement(int blinkInterval)
{
	auto wSize = Application::Instance().GetViewport().GetSize();

	const int LINE_SPACING = 130;
	int yPos = logoPos.y + LINE_SPACING;

	auto data = Application::Instance().GetResultData();

	if (data.gameClear)
	{
		DrawFormatString(logoPos.x, yPos, 0xffffff, L"Clear Time : %4d:%d", data.playTime / 60, (data.playTime % 60));

		yPos += +LINE_SPACING;
	}

	if (blinkCount_ < blinkInterval)
	{
		DrawRotaGraph(wSize.w / 2.0f, wSize.h * 0.9f, 0.7, 0.0, pressStartImage_, true);
	}

	if (blinkCount_ > blinkInterval + BLINK_TIME)
	{
		blinkCount_ = 0;
	}
}

ResultScene::~ResultScene()
{
}

void ResultScene::Update(const Input& input)
{
	blinkCount_++;
	(this->*updater_)(input);
	Scene::UpdateFade(input);
}

void ResultScene::Draw()
{
	(this->*drawer_)();

	Scene::PostDraw();
}

void ResultScene::EndFadeIn()
{
	updater_ = &ResultScene::NormalUpdate;
	drawer_ = &ResultScene::NormalDraw;
}

void ResultScene::EndFadeOut()
{
	controller_.ChangeScene(new TitleScene(controller_));
}
