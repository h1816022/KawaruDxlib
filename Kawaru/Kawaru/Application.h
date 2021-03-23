#pragma once
#include <memory>
#include <unordered_map>
#include "Tools/Geometry.h"
#include "ResultData.h"

class SceneController;
struct Size;

/// <summary>
/// ��ʃf�[�^
/// </summary>
class Viewport
{
public:
	/// <summary>
	/// ��ʃT�C�Y�̎擾
	/// </summary>
	/// <returns>��ʃT�C�Y</returns>
	Size GetSize()const;
};

/// <summary>
/// ��A�̃v���O�����̎��s���s���N���X
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
	/// ����������
	/// </summary>
	/// <returns></returns>
	bool Initialize();

	/// <summary>
	/// �Q�[�����[�v
	/// </summary>
	void Run();

	/// <summary>
	/// �I������
	/// </summary>
	void Terminate();

	~Application();

	/// <summary>
	/// �A�v���P�[�V�������I��������
	/// </summary>
	void Exit();

	/// <summary>
	/// ��ʃf�[�^�̎擾
	/// </summary>
	/// <returns>��ʃf�[�^</returns>
	const Viewport& GetViewport()const;

	/// <summary>
	/// �L�[�̕\�������C���f�b�N�X�ŎQ�Ƃł���悤�ɓ���Ă���
	/// </summary>
	void InitVKeyName();

	/// <summary>
	/// �L�[�̖��O���擾
	/// </summary>
	/// <param name="index">�L�[�̃C���f�b�N�X</param>
	/// <returns>�L�[�̖��O</returns>
	const wchar_t* GetVKeyName(int index);

	/// <summary>
	/// �L�[�̖��O��S�Ď擾
	/// </summary>
	/// <returns>�L�[�̖��O���X�g</returns>
	const std::unordered_map<int, const wchar_t*>& GetAllVKeyName()const;

	/// <summary>
	/// ���U���g�̃f�[�^�̋L�^���n�߂�
	/// </summary>
	void StartRecording(bool reset);

	/// <summary>
	/// ���U���g�̃f�[�^�̋L�^���~�߂�
	/// </summary>
	void StopRecording();

	/// <summary>
	/// ���U���g�Ɏg�p����L�^�f�[�^���擾
	/// </summary>
	/// <returns>�L�^�����f�[�^</returns>
	const ResultData& GetResultData()const;

	/// <summary>
	/// �Q�[���N���A�ɂ���
	/// </summary>
	/// <returns>true : �Q�[���N���A�ɂȂ��� false : ���ɂȂ��Ă������A�Q�[���I�[�o�[�ɂȂ��Ă���</returns>
	bool SetIsGameClear();

	/// <summary>
	/// �Q�[���I�[�o�[�ɂ���
	/// </summary>
	/// <returns>true : �Q�[���I�[�o�[�ɂȂ��� false : ���ɂȂ��Ă������A�Q�[���N���A���Ă���</returns>
	bool SetIsGameOver();

	/// <summary>
	/// �Q�[���N���A���Ă���
	/// </summary>
	/// <returns>true : �Q�[���N���A���Ă���</returns>
	bool CheckIsGameClear()const;

	/// <summary>
	/// �Q�[���I�[�o�[�ɂȂ��Ă��邩
	/// </summary>
	/// <returns>true : �Q�[���I�[�o�[�ɂȂ��Ă���</returns>
	bool CheckIsGameOver()const;

private:
	Application();

	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

	std::unique_ptr<SceneController> sceneController_;

	Viewport viewport_;

	// �A�v���P�[�V�����I���t���O
	bool isExit = false;

	std::unordered_map<int, const wchar_t*> vKeyName_;

	// �v���C���Ԃ��J�E���g����
	bool countTime_ = false;

	ResultData resultData_;
};