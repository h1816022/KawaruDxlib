#pragma once
#include <memory>
#include <vector>
#include <array>
#include "Scene.h"
#include "../Geometry.h"

class TitleScene;
class Stage;
class Player;

/// <summary>
/// プレイ中のシーン
/// </summary>
class GameplayingScene :
	public Scene
{
	friend SceneController;
	friend TitleScene;

public:
	~GameplayingScene();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override final;

private:
	GameplayingScene(SceneController& controller);

	/// <summary>
	/// ゲーム中通常の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void NormalUpdate(const Input& input);

	/// <summary>
	/// フェードイン中の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void FadeInUpdate(const Input& input);

	/// <summary>
	/// フェードアウト中の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void FadeOutUpdate(const Input& input);

	/// <summary>
	/// このシーンから他シーンへ切り替わる時の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void ChangeSceneUpdate(const Input& input);

	/// <summary>
	/// フェードインが終わった時の処理
	/// </summary>
	void EndFadeIn()override final;

	/// <summary>
	/// フェードアウトが終わった時の処理
	/// </summary>
	void EndFadeOut()override final;

	using UpdateFunc = void (GameplayingScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// ゲーム中通常の描画処理
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// フェードイン中の描画処理
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// フェードイン中の描画処理
	/// </summary>
	void FadeOutDraw();

	void UpdateVibration();

	void UpdateHUD();

	int vibrationCount_ = 0;

	using DrawFunc = void (GameplayingScene::*)();
	DrawFunc drawer_;

	uint32_t nowCount_ = 0;

	const Size vpSize_;

	std::shared_ptr<Player> player_;

	std::shared_ptr<Stage> stage_;

	std::array<float, 300> waveform_;
	int waveformIndex_ = 0;
	float nowWaveformRate_ = 0.0f;

	int candleHUDHandle_ = -1;
	float candleHUDAngle_ = 0.0f;
};