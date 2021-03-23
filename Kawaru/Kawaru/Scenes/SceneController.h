
#pragma once
#include <memory>
#include <deque>
#include <functional>
#include <vector>

class Scene;
class Input;
class Application;

/// <summary>
/// シーンの管理クラス
/// </summary>
class SceneController
{
	friend Application;

public:
	/// <summary>
	/// 現在の一番上の(更新処理されている)シーンを終了し、新しいシーンを生成
	/// </summary>
	/// <param name="newScene">新しいシーン</param>
	void ChangeScene(Scene* newScene);

	/// <summary>
	/// 現在のシーンをキープしたまま新しいシーンを追加
	/// </summary>
	/// <param name="newScene">新しいシーン</param>
	void PushScene(Scene* newScene);

	/// <summary>
	/// 現在の一番上の(更新処理されている)シーンから指定数分を終了
	/// </summary>
	void PopScene(unsigned int num = 1);

	/// <summary>
	/// 現在のシーンを全て終了し、新しいシーンを生成
	/// </summary>
	/// <param name="newScene">新しいシーン</param>
	void CleanChangeScene(Scene* newScene);

	/// <summary>
	/// 現在のシーンをすべて終了
	/// </summary>
	void ClearScenes();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void Update(const Input& input);

	/// <summary>
	/// 描画情報
	/// </summary>
	void Draw();

	/// <summary>
	/// 現在上から指定番目にあるシーンを取得
	/// </summary>
	/// <returns>指定番目のシーン</returns>
	std::shared_ptr<Scene> GetSceneFromTop(int num);

private:
	SceneController();

	std::deque<std::shared_ptr<Scene>> scenes_;

	std::vector<std::function<void(void)>> postDrawExecuter_;
};