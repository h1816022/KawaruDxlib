#pragma once
#include "Scene.h"

class SceneController;
class GameplayingScene;

/// <summary>
/// プレイ結果のシーン
/// </summary>
class ResultScene :
	public Scene
{
	friend SceneController;
	friend GameplayingScene;

public:
	~ResultScene();

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
	ResultScene(SceneController& controller);

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
	void FadeOutUpdate(const Input& input);

	using UpdateFunc = void (ResultScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// 通常時の描画処理
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// リザルトの項目の描画
	/// </summary>
	/// <param name="blinkInterval"></param>
	void DrawResultElement(int blinkInterval);

	/// <summary>
	/// フェードイン時の描画処理
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// フェードアウト時の描画処理
	/// </summary>
	void FadeOutDraw();

	using DrawFunc = void (ResultScene::*)();
	DrawFunc drawer_;

	int blinkCount_;

	uint32_t fadeTime_;

	Vector2f logoPos_;

	int resultImage_ = -1;
	int pressStartImage_ = -1;

	int okSE_ = -1;
};