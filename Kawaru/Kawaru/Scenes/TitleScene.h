#pragma once
#include "Scene.h"

class GameplayingScene;
class ResultScene;
class PauseScene;

/// <summary>
/// �^�C�g����ʂ̃V�[��
/// </summary>
class TitleScene :
	public Scene
{
	friend SceneController;
	friend ResultScene;
	friend PauseScene;

public:
	~TitleScene();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input)override final;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override final;

	/// <summary>
	/// �t�F�[�h�C���I�����̏���
	/// </summary>
	void EndFadeIn()override final;

	/// <summary>
	/// �t�F�[�h�A�E�g�I�����̏���
	/// </summary>
	void EndFadeOut()override final;

private:
	TitleScene(SceneController& controller);

	/// <summary>
	/// �ʏ펞�̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void NormalUpdate(const Input& input);

	/// <summary>
	/// �t�F�[�h�C�����̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void FadeinUpdate(const Input& input);

	/// <summary>
	/// �t�F�[�h�A�E�g���̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void FadeoutUpdate(const Input& input);

	using UpdateFunc = void (TitleScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// �ʏ펞�̕`�揈��
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// �t�F�[�h�C�����̕`�揈��
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// �t�F�[�h�A�E�g���̕`�揈��
	/// </summary>
	void FadeOutDraw();

	using DrawFunc = void (TitleScene::*)();
	DrawFunc drawer_;

	int blinkCount_;

	uint32_t fadeTime_;

	int titleImage_ = -1;
	int pressStartImage_ = -1;

	int okSE_ = -1;
};