#pragma once
#include <string>
#include <functional>
#include "MenuSceneBase.h"
#include "../Geometry.h"

class GameplayingScene;
class PauseScene;

/// <summary>
/// 指定秒数遅らせて実行させたい処理の登録用
/// </summary>
struct DelayFuncData
{
	using Func_t = void (PauseScene::*)();

	DelayFuncData(Func_t f, uint32_t end) :func(f), endCount(end) {}
	Func_t func;

	uint32_t nowCount = 0;
	uint32_t endCount;
};

class PauseScene :
	public MenuSceneBase
{
	friend SceneController;
	friend GameplayingScene;

public:
	~PauseScene();

	void Update(const Input& input)override final;

	void Draw()override final;

private:
	PauseScene(SceneController& controller);

	/// <summary>
	/// タイトル画面に戻る
	/// </summary>
	void ReturnToTitle();

	std::vector<DelayFuncData> delayFunctions_;

	int32_t delayCount_ = 0;
};

