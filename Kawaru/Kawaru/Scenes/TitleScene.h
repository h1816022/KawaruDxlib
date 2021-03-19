#pragma once
#include "Scene.h"

class GameplayingScene;
class ResultScene;
class PauseScene;

/// <summary>
/// タイトル画面のシーン
/// </summary>
class TitleScene :
	public Scene
{
	friend SceneController;
	friend ResultScene;
	friend PauseScene;

public:
	~TitleScene();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override final;

	/// <summary>
	/// フェードイン終了時の処理
	/// </summary>
	void EndFadeIn()override final;

	/// <summary>
	/// フェードアウト終了時の処理
	/// </summary>
	void EndFadeOut()override final;

private:
	TitleScene(SceneController& controller);

	/// <summary>
	/// 通常時の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void NormalUpdate(const Input& input);

	/// <summary>
	/// フェードイン時の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void FadeinUpdate(const Input& input);

	/// <summary>
	/// フェードアウト時の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void FadeoutUpdate(const Input& input);

	using UpdateFunc = void (TitleScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// 通常時の描画処理
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// フェードイン時の描画処理
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// フェードアウト時の描画処理
	/// </summary>
	void FadeOutDraw();

	using DrawFunc = void (TitleScene::*)();
	DrawFunc drawer_;

	int blinkCount_;

	uint32_t fadeTime_;

	int titleImage_ = -1;
	int pressStartImage_ = -1;

	int okSE_ = -1;
};