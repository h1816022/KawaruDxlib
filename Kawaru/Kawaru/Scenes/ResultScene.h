#pragma once
#include "Scene.h"

class SceneController;
class GameplayingScene;

/// <summary>
/// �v���C���ʂ̃V�[��
/// </summary>
class ResultScene :
	public Scene
{
	friend SceneController;
	friend GameplayingScene;

public:
	~ResultScene();

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
	ResultScene(SceneController& controller);

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
	void FadeOutUpdate(const Input& input);

	using UpdateFunc = void (ResultScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// �ʏ펞�̕`�揈��
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// ���U���g�̍��ڂ̕`��
	/// </summary>
	/// <param name="blinkInterval"></param>
	void DrawResultElement(int blinkInterval);

	/// <summary>
	/// �t�F�[�h�C�����̕`�揈��
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// �t�F�[�h�A�E�g���̕`�揈��
	/// </summary>
	void FadeOutDraw();

	using DrawFunc = void (ResultScene::*)();
	DrawFunc drawer_;

	int blinkCount_;

	uint32_t fadeTime_;

	Vector2f logoPos_;

	int resultImage_ = -1;
	int pressStartImage_ = -1;

	int okSE_ = -1;
};