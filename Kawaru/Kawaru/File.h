#pragma once
#include <string>
#include <vector>

class FileManager;

class File
{
	friend FileManager;

public:
	// �t�@�C���̎��
	enum class FILE_TYPE
	{
		Image,
		Sound
	};

	~File();

	int GetHandle()const;

private:
	File(int handle, FILE_TYPE type);

	int handle_;

	FILE_TYPE type_;
};