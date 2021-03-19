#pragma once
#include <string>
#include <vector>

class FileManager;

/// <summary>
/// �O�����烍�[�h����f�[�^
/// </summary>
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

	/// <summary>
	/// �n���h���̎擾
	/// </summary>
	/// <returns>�f�[�^�̃n���h��</returns>
	int GetHandle()const;

private:
	File(int handle, FILE_TYPE type);

	int handle_;

	FILE_TYPE type_;
};