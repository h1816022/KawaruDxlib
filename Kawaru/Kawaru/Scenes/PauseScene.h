#pragma once
#include <string>
#include <functional>
#include "MenuSceneBase.h"
#include "../Geometry.h"

class GameplayingScene;
class PauseScene;

/// <summary>
/// w’è•b”’x‚ç‚¹‚ÄÀs‚³‚¹‚½‚¢ˆ—‚Ì“o˜^—p
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
	/// ƒ^ƒCƒgƒ‹‰æ–Ê‚É–ß‚é
	/// </summary>
	void ReturnToTitle();

	std::vector<DelayFuncData> delayFunctions_;

	int32_t delayCount_ = 0;
};

