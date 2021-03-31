#include <DxLib.h>
#include "GameplayingScene.h"
#include "SceneController.h"
#include "ResultScene.h"
#include "../Application.h"
#include "../Systems/Input.h"
#include "../Actors/Players/Player.h"
#include "../Actors/Camera.h"
#include "../Actors/Stage.h"
#include "../Actors/Gimmicks/Goal.h"
#include "../NavMesh/NavMesh.h"
#include "../Actors/Ghost.h"
#include "PauseScene.h"
#include "../Systems/File.h"
#include "../Systems/FileManager.h"

namespace
{
	// フェードアウトにかける時間
	constexpr uint32_t FADE_OUT_INTERVAL = 150;

	// 振動の最小インターバル
	constexpr int MIN_VIBRATION_INTERVAL = 10;

	// 振動の最大インターバル
	constexpr int MAX_VIBRATION_INTERVAL = 120;

	// ゲームプレイ中のフォントサイズ
	constexpr int DURING_GAME_PLAY_FONT_SIZE = 35;
}

GameplayingScene::GameplayingScene(SceneController& controller) :
	Scene(controller),
	updater_(&GameplayingScene::FadeInUpdate),
	drawer_(&GameplayingScene::FadeInDraw),
	vpSize_(Application::Instance().GetViewport().GetSize())
{
	SetFontSize(DURING_GAME_PLAY_FONT_SIZE);

	StartFade(FADE_MODE::In);

	auto navMesh = std::make_shared<NavMesh>(*this);
	AddActors(navMesh);

	stage_ = std::make_shared<Stage>(*this, *navMesh);
	AddActors(stage_);

	auto camera = std::make_shared<Camera>(*this, *stage_);
	AddActors(camera);

	auto ghost = std::make_shared<Ghost>(*this, *camera, *stage_);
	AddActors(ghost);

	player_ = std::make_shared<Player>(*this, *camera, *ghost, *stage_);
	AddActors(player_);

	stage_->SetPlayer(player_);
	stage_->InitGimmicks();

	camera->SetPlayer(player_);

	ghost->InitPos();

	waveform_.fill(0.0f);

	candleHUDHandle_ = MV1LoadModel(L"Resources/Models/Candle.mqo");

	constexpr float CANDLE_HUD_SCALE = 0.003f;
	MV1SetScale(candleHUDHandle_, VECTOR(CANDLE_HUD_SCALE, CANDLE_HUD_SCALE, CANDLE_HUD_SCALE));

	bgm_ = FileManager::Instance().Load(L"Resources/Sounds/BGM.mp3")->GetHandle();
	ChangeVolumeSoundMem(128, bgm_);
	PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP);
}

void GameplayingScene::NormalUpdate(const Input& input)
{
	auto& app = Application::Instance();

	if (app.CheckIsGameClear() || app.CheckIsGameOver())
	{
		StartFade(FADE_MODE::Out, FADE_OUT_INTERVAL);
		updater_ = &GameplayingScene::FadeOutUpdate;
		drawer_ = &GameplayingScene::FadeOutDraw;

		app.StopRecording();

		return;
	}

	if (input.IsTriggered("Pause"))
	{
		controller_.PushScene(new PauseScene(controller_));
		return;
	}

	UpdateVibration();

	Scene::UpdateActors(input);

	UpdateHUD();
}

void GameplayingScene::FadeInUpdate(const Input& input)
{
	Scene::UpdateActors(input);
}

void GameplayingScene::FadeOutUpdate(const Input& input)
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
	Application::Instance().StartRecording(true);
}

void GameplayingScene::EndFadeOut()
{
	controller_.ChangeScene(new ResultScene(controller_));
}

void GameplayingScene::NormalDraw()
{
	int nowTime = Application::Instance().GetResultData().playTime;

	constexpr int HUD_OFFSET = 30;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(HUD_OFFSET, HUD_OFFSET, HUD_OFFSET + 143, HUD_OFFSET + 45, 0x333333, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawFormatString(HUD_OFFSET + 5, HUD_OFFSET + 5, 0xffffff, L"%04d:%02d", nowTime / 60, (nowTime % 60));

	const int WAVEFORM_SIZE = waveform_.size();

	auto wSize = Application::Instance().GetViewport().GetSize();

	int red = static_cast<int>(255.0f * (1.0f - nowWaveformRate_ / 2.0f));
	unsigned int color = GetColor(red, 255 - red, 0);

	red = static_cast<int>(static_cast<float>(red) * 0.7f);

	unsigned int color2 = GetColor(red, static_cast<int>(255.0f * 0.7f) - red, 0);

	for (int i = 0; i < WAVEFORM_SIZE - 1; ++i)
	{
		DrawLine(2 + wSize.w - HUD_OFFSET - WAVEFORM_SIZE + i, 2 + HUD_OFFSET * 2.0f + (20 - static_cast<int>(waveform_[(i + 1 + waveformIndex_) % WAVEFORM_SIZE] * 20)),
			2 + wSize.w - HUD_OFFSET - WAVEFORM_SIZE + i + 1, 2 + HUD_OFFSET * 2.0f + (20 - static_cast<int>(waveform_[(i + 2 + waveformIndex_) % WAVEFORM_SIZE] * 20)), color2);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(wSize.w - HUD_OFFSET - WAVEFORM_SIZE, HUD_OFFSET,
		wSize.w - HUD_OFFSET, HUD_OFFSET * 2.0f + 40, 0x050533, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	for (int i = 0; i < WAVEFORM_SIZE - 1; ++i)
	{
		DrawLine(wSize.w - HUD_OFFSET - WAVEFORM_SIZE + i, HUD_OFFSET * 2.0f + (20 - static_cast<int>(waveform_[(i + 1 + waveformIndex_) % WAVEFORM_SIZE] * 20)),
			wSize.w - HUD_OFFSET - WAVEFORM_SIZE + i + 1, HUD_OFFSET * 2.0f + (20 - static_cast<int>(waveform_[(i + 2 + waveformIndex_) % WAVEFORM_SIZE] * 20)), color);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

	DrawBox(wSize.w - HUD_OFFSET - WAVEFORM_SIZE, 110, wSize.w - HUD_OFFSET - WAVEFORM_SIZE + 170, 170, 0x249105, true);

	int callNum = player_->GetCallNum();

	DrawFormatString(1000, 125, 0xffffff, L"×%03d", callNum);
	DrawFormatString(1000, 125, 0xffffff, L"×%3d", callNum);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	MV1DrawModel(candleHUDHandle_);
}

void GameplayingScene::FadeInDraw()
{
}

void GameplayingScene::FadeOutDraw()
{
}

GameplayingScene::~GameplayingScene()
{
	StopSoundMem(bgm_);
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

void GameplayingScene::UpdateVibration()
{
	constexpr float NEAR_DISTANCE = 2000.0f;
	constexpr float FAR_DISTANCE = 8000.0f;

	float distance = GetLength(VSub(player_->GetPos(), stage_->GetGoal()->GetPos()));

	if (distance - NEAR_DISTANCE <= 0.0f)
	{
		nowWaveformRate_ = 0.0f;
	}
	else if (FAR_DISTANCE - distance <= 0.0f)
	{
		nowWaveformRate_ = 1.0f;
	}
	else
	{
		nowWaveformRate_ = (distance - NEAR_DISTANCE) / (FAR_DISTANCE - NEAR_DISTANCE);
	}

	int oldWaveformIndex = waveformIndex_;
	waveformIndex_ = (waveformIndex_ == waveform_.size() - 1) ? 0 : ++waveformIndex_;

	if (++vibrationCount_ >= static_cast<int>(Lerp(static_cast<float>(MIN_VIBRATION_INTERVAL), static_cast<float>(MAX_VIBRATION_INTERVAL), nowWaveformRate_)))
	{
		vibrationCount_ = 0;

		StartJoypadVibration(DX_INPUT_PAD1, 500, 100);

		waveform_[waveformIndex_] = (1.0f - nowWaveformRate_) * 2.0f;
	}
	else
	{
		float randomNum = static_cast<float>(GetRand(100)) / 100.0f;
		waveform_[waveformIndex_] = waveform_[oldWaveformIndex] * Lerp(0.6f, 0.8f, randomNum) + Lerp(-0.1f, 0.1f, randomNum);
	}
}

void GameplayingScene::UpdateHUD()
{
	MV1SetPosition(candleHUDHandle_, ConvScreenPosToWorldPos(VGet(980.0f, 160.0f, 0.1f)));
	MV1SetRotationXYZ(candleHUDHandle_, VGet(0.0f, 0.0f, 0.0f));

	MV1SetRotationXYZ(candleHUDHandle_, VGet(0.0f, candleHUDAngle_, 0.0f));

	candleHUDAngle_ += 0.03f;
	candleHUDAngle_ = (candleHUDAngle_ > DX_PI_F * 2) ? (candleHUDAngle_ - DX_PI_F * 2) : candleHUDAngle_;
}
