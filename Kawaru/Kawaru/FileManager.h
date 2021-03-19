#pragma once
#include <unordered_map>
#include <memory>
#include <list>

class File;

// 遅れて再生する効果音データ
struct DelaySoundData
{
	DelaySoundData(int handle, int time) :
		soundHandle(handle), delayTime(time), nowTime(0) {}

	// ハンドル
	const int soundHandle;
	
	// 待つ時間
	const int delayTime;
	
	// 現在待った時間
	int nowTime;
};

/// <summary>
/// 外部からロードするファイルデータ管理クラス
/// </summary>
class FileManager
{
public:
	~FileManager();

	/// <summary>
	/// シングルトンのインスタンスを返す
	/// </summary>
	/// <returns></returns>
	static FileManager& Instance()
	{
		static FileManager instance;
		return instance;
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ファイルのロード
	/// </summary>
	/// <param name="filePath">ファイルのパス</param>
	/// <returns>ハンドル</returns>
	std::shared_ptr<File> Load(const wchar_t* filePath);

	/// <summary>
	/// 登録しているファイルデータをすべてクリア
	/// </summary>
	void DeleteAllResources();

	/// <summary>
	/// 指定時間待って音を再生
	/// </summary>
	/// <param name="handle">サウンドファイルのハンドル</param>
	/// <param name="delayTime">待つ時間</param>
	void DelayPlaySound(int handle, int delayTime);

private:
	FileManager();
	FileManager(const FileManager&) = delete;
	void operator=(const FileManager&) = delete;

	std::unordered_map<std::wstring, std::shared_ptr<File>> resourceTable_;

	// 遅れて再生する効果音リスト
	std::list<DelaySoundData> delaySounds_;
};