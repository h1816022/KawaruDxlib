#pragma once
#include <memory>
#include <unordered_map>
#include "Geometry.h"

class SceneController;
struct Size;

class Viewport
{
public:
	Size GetSize()const;
};

class Application
{
public:
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <returns></returns>
	bool Initialize();

	/// <summary>
	/// ゲームループ
	/// </summary>
	void Run();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Terminate();

	~Application();

	/// <summary>
	/// アプリケーションを終了させる
	/// </summary>
	void Exit();

	const Viewport& GetViewport()const;

	/// <summary>
	/// キーの表示名をインデックスで参照できるように入れておく
	/// </summary>
	void InitVKeyName();

	const wchar_t* GetVKeyName(int index);
	const std::unordered_map<int, const wchar_t*>& GetAllVKeyName()const;

private:
	Application();

	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

	std::unique_ptr<SceneController> sceneController_;

	Viewport viewport_;

	// アプリケーション終了フラグ
	bool isExit = false;

	uint32_t nowCount_ = 0;

	std::unordered_map<int, const wchar_t*> vKeyName_;
};