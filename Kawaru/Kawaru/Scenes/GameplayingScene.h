#pragma once
#include <memory>
#include <vector>
#include "Scene.h"
#include "../Geometry.h"

class TitleScene;
class Stage;

struct Cube
{
	VECTOR center;
	VECTOR radius;
};

class GameplayingScene :
	public Scene
{
	friend SceneController;
	friend TitleScene;

public:
	~GameplayingScene();

	void Update(const Input& input)override final;

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

	using DrawFunc = void (GameplayingScene::*)();
	DrawFunc drawer_;

	uint32_t nowCount_ = 0;

	const Size vpSize_;

	Cube testCubes[2];
};