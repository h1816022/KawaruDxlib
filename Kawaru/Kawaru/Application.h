#pragma once
#include <memory>
#include <unordered_map>
#include "Tools/Geometry.h"
#include "ResultData.h"

class SceneController;
struct Size;

/// <summary>
/// 画面データ
/// </summary>
class Viewport
{
public:
	/// <summary>
	/// 画面サイズの取得
	/// </summary>
	/// <returns>画面サイズ</returns>
	Size GetSize()const;
};

/// <summary>
/// 一連のプログラムの実行を行うクラス
/// </summary>
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

	/// <summary>
	/// 画面データの取得
	/// </summary>
	/// <returns>画面データ</returns>
	const Viewport& GetViewport()const;

	/// <summary>
	/// キーの表示名をインデックスで参照できるように入れておく
	/// </summary>
	void InitVKeyName();

	/// <summary>
	/// キーの名前を取得
	/// </summary>
	/// <param name="index">キーのインデックス</param>
	/// <returns>キーの名前</returns>
	const wchar_t* GetVKeyName(int index);

	/// <summary>
	/// キーの名前を全て取得
	/// </summary>
	/// <returns>キーの名前リスト</returns>
	const std::unordered_map<int, const wchar_t*>& GetAllVKeyName()const;

	/// <summary>
	/// リザルトのデータの記録を始める
	/// </summary>
	void StartRecording(bool reset);

	/// <summary>
	/// リザルトのデータの記録を止める
	/// </summary>
	void StopRecording();

	/// <summary>
	/// リザルトに使用する記録データを取得
	/// </summary>
	/// <returns>記録したデータ</returns>
	const ResultData& GetResultData()const;

	/// <summary>
	/// ゲームクリアにする
	/// </summary>
	/// <returns>true : ゲームクリアになった false : 既になっていたか、ゲームオーバーになっている</returns>
	bool SetIsGameClear();

	/// <summary>
	/// ゲームオーバーにする
	/// </summary>
	/// <returns>true : ゲームオーバーになった false : 既になっていたか、ゲームクリアしている</returns>
	bool SetIsGameOver();

	/// <summary>
	/// ゲームクリアしていか
	/// </summary>
	/// <returns>true : ゲームクリアしている</returns>
	bool CheckIsGameClear()const;

	/// <summary>
	/// ゲームオーバーになっているか
	/// </summary>
	/// <returns>true : ゲームオーバーになっている</returns>
	bool CheckIsGameOver()const;

private:
	Application();

	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

	std::unique_ptr<SceneController> sceneController_;

	Viewport viewport_;

	// アプリケーション終了フラグ
	bool isExit = false;

	std::unordered_map<int, const wchar_t*> vKeyName_;

	// プレイ時間をカウントする
	bool countTime_ = false;

	ResultData resultData_;
};