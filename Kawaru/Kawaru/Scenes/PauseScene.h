#pragma once
#include <string>
#include <functional>
#include "MenuSceneBase.h"
#include "../Tools/Geometry.h"

class GameplayingScene;
class PauseScene;

// �w��b���x�点�Ď��s�������������̓o�^�p
struct DelayFuncData
{
	using Func_t = void (PauseScene::*)();

	DelayFuncData(Func_t f, uint32_t end) :func(f), endCount(end) {}
	Func_t func;

	uint32_t nowCount = 0;
	uint32_t endCount;
};

/// <summary>
/// �v���C���ɒ��f�������̃V�[��
/// </summary>
class PauseScene :
	public MenuSceneBase
{
	friend SceneController;
	friend GameplayingScene;

public:
	~PauseScene();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override final;

private:
	PauseScene(SceneController& controller);

	/// <summary>
	/// �^�C�g����ʂɖ߂�
	/// </summary>
	void ReturnToTitle();

	std::vector<DelayFuncData> delayFunctions_;

	int32_t delayCount_ = 0;
};

