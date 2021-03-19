#pragma once
#include <string>
#include <vector>

class FileManager;

/// <summary>
/// 外部からロードするデータ
/// </summary>
class File
{
	friend FileManager;

public:
	// ファイルの種類
	enum class FILE_TYPE
	{
		Image,
		Sound
	};

	~File();

	/// <summary>
	/// ハンドルの取得
	/// </summary>
	/// <returns>データのハンドル</returns>
	int GetHandle()const;

private:
	File(int handle, FILE_TYPE type);

	int handle_;

	FILE_TYPE type_;
};