#pragma once
#include <array>
#include <unordered_map>
#include <string>

// 入力情報を保存するバッファサイズ
constexpr int INPUT_RECORD_SIZE = 2;

// 入力デバイスの種類
enum class PeripheralType
{
	keyboard,
	gamepad
};

struct PeripheralInfo
{
	PeripheralType type;
	int index;
};

enum class AnalogInputType
{
	LEFT,
	RIGHT
};

struct AnalogInputData
{
	AnalogInputData():
	horizontal(0.0f), vertical(0.0f) {}

	AnalogInputData(float hori, float vert) :
		horizontal(hori), vertical(vert) {}

	float Length()
	{
		return sqrtf(horizontal * horizontal + vertical * vertical);
	};

	float horizontal;
	float vertical;
};

class KeyConfigScene;
class GameplayingScene;

class Input
{
	friend KeyConfigScene;
	friend GameplayingScene;
public:
	Input();

	void Update();

	/// <summary>
	/// 現在押されているかを調べて取得
	/// </summary>
	/// <param name="eventName">名前</param>
	/// <param name="keyState">キーの入力情報</param>
	/// <param name="padState">パッドの入力情報</param>
	/// <returns>true : 押されている</returns>
	bool CheckPressed(const char* eventName, const char* keyState, int padState);

	/// <summary>
	/// 押されているか
	/// </summary>
	/// <param name="cmd">名前</param>
	/// <returns>true : 押されている</returns>
	bool IsPressed(const char* cmd)const;

	/// <summary>
	/// 押された瞬間か
	/// </summary>
	/// <param name="cmd">名前</param>
	/// <returns>true : このフレームに押された</returns>
	bool IsTriggered(const char* cmd)const;

	/// <summary>
	/// 離された瞬間か
	/// </summary>
	/// <param name="cmd">名前</param>
	/// <returns>true : このフレームに離された</returns>
	bool IsReleased(const char* cmd)const;

	AnalogInputData GetAnalogInput(AnalogInputType type)const;

private:
	/// <summary>
	/// 次フレームの入力バッファインデックスを返す
	/// </summary>
	/// <returns>次フレームの入力バッファインデックス</returns>
	int GetNextInputBufferIndex();

	/// <summary>
	/// 前フレームの入力バッファインデックスを返す
	/// </summary>
	/// <returns>前フレームの入力バッファインデックス</returns>
	int GetPrevInputBufferIndex()const;

	/// <summary>
	/// 現在の入力情報への参照を返す
	/// </summary>
	/// <param name="cmd">入力情報を見たいコマンドの名前</param>
	/// <returns>true : 現在押されている</returns>
	bool& CurrentInput(const std::string cmd);

	/// <summary>
	/// 入力情報の更新
	/// </summary>
	void UpdateCurrentInput();

	/// <summary>
	/// 現在の入力情報を返す
	/// </summary>
	/// <param name="cmd">入力情報を見たいコマンドの名前</param>
	/// <returns>true : 現在押されている</returns>
	bool GetCurrentInput(const std::string cmd)const;

	/// <summary>
	/// 前フレームの入力情報を返す
	/// </summary>
	/// <param name="cmd">入力情報を見たいコマンドの名前</param>
	/// <returns>true : 直前に押されていた</returns>
	bool PrevInput(const std::string cmd)const;

	/// <summary>
	/// 入力のマッピングを設定
	/// </summary>
	/// <param name="eventName">任意の名前</param>
	/// <param name="peri">デバイスの種類などのデータ</param>
	void RegistAcceptPeripheral(const char* eventName, const std::vector<PeripheralInfo>& peri);

	/// <summary>
	/// 生キーボード情報を持ってくる
	/// </summary>
	/// <returns>生キーボード情報</returns>
	const std::vector<char>& GetRawKeyboardState()const;

	/// <summary>
	/// 生パッド情報を持ってくる
	/// ただし1番
	/// </summary>
	/// <returns>生パッド情報</returns>
	const int GetRawPadState()const;

	// 現在の入力情報バッファのインデックス
	int currentInputIndex_ = 0;

	// 現在の入力情報バッファ
	using InputStateTalbe = std::unordered_map<std::string, bool>;
	std::array<InputStateTalbe, INPUT_RECORD_SIZE> inputStateTable_;

	using PeripheralReferenceTable_t = std::unordered_map<std::string, std::vector<PeripheralInfo>>;

	/// <summary>
	/// 外側から入力テーブルをいじる用
	/// </summary>
	/// <param name="prt">変更するテーブル</param>
	void SetPeripheralReferenceTable(const PeripheralReferenceTable_t& prt)const;

	mutable PeripheralReferenceTable_t peripheralReferenceTable_;
	mutable PeripheralReferenceTable_t defaultPeripheralReferenceTable_;

	mutable std::vector<char> rawKeyState_;
	mutable int rawPadState_;
};

class InputListener
{
public:
	virtual void Notify(const Input& input) = 0;
};