#pragma once
#include "Scene.h"

class GameplayingScene;
class PauseScene;

class TitleScene :
	public Scene
{
	friend SceneController;
	friend GameplayingScene;
	friend PauseScene;

public:
	~TitleScene();

	void Update(const Input& input)override final;
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

	int count_;

	uint32_t fadeTime_;
};