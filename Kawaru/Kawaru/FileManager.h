#pragma once
#include <unordered_map>
#include <memory>
#include <list>

class File;

// �x��čĐ�������ʉ��f�[�^
struct DelaySoundData
{
	DelaySoundData(int handle, int time) :
		soundHandle(handle), delayTime(time), nowTime(0) {}

	// �n���h��
	const int soundHandle;
	
	// �҂���
	const int delayTime;
	
	// ���ݑ҂�������
	int nowTime;
};

/// <summary>
/// �O�����烍�[�h����t�@�C���f�[�^�Ǘ��N���X
/// </summary>
class FileManager
{
public:
	~FileManager();

	/// <summary>
	/// �V���O���g���̃C���X�^���X��Ԃ�
	/// </summary>
	/// <returns></returns>
	static FileManager& Instance()
	{
		static FileManager instance;
		return instance;
	}

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �t�@�C���̃��[�h
	/// </summary>
	/// <param name="filePath">�t�@�C���̃p�X</param>
	/// <returns>�n���h��</returns>
	std::shared_ptr<File> Load(const wchar_t* filePath);

	/// <summary>
	/// �o�^���Ă���t�@�C���f�[�^�����ׂăN���A
	/// </summary>
	void DeleteAllResources();

	/// <summary>
	/// �w�莞�ԑ҂��ĉ����Đ�
	/// </summary>
	/// <param name="handle">�T�E���h�t�@�C���̃n���h��</param>
	/// <param name="delayTime">�҂���</param>
	void DelayPlaySound(int handle, int delayTime);

private:
	FileManager();
	FileManager(const FileManager&) = delete;
	void operator=(const FileManager&) = delete;

	std::unordered_map<std::wstring, std::shared_ptr<File>> resourceTable_;

	// �x��čĐ�������ʉ����X�g
	std::list<DelaySoundData> delaySounds_;
};