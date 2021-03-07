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

	const Viewport& GetViewport()const;

	/// <summary>
	/// �L�[�̕\�������C���f�b�N�X�ŎQ�Ƃł���悤�ɓ���Ă���
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

	// �A�v���P�[�V�����I���t���O
	bool isExit = false;

	uint32_t nowCount_ = 0;

	std::unordered_map<int, const wchar_t*> vKeyName_;
};