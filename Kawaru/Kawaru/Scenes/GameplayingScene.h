#pragma once
#include <memory>
#include <vector>
#include <array>
#include "Scene.h"
#include "../Geometry.h"

class TitleScene;
class Stage;
class Player;

/// <summary>
/// �v���C���̃V�[��
/// </summary>
class GameplayingScene :
	public Scene
{
	friend SceneController;
	friend TitleScene;

public:
	~GameplayingScene();

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
	GameplayingScene(SceneController& controller);

	/// <summary>
	/// �Q�[�����ʏ�̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void NormalUpdate(const Input& input);

	/// <summary>
	/// �t�F�[�h�C�����̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void FadeInUpdate(const Input& input);

	/// <summary>
	/// �t�F�[�h�A�E�g���̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void FadeOutUpdate(const Input& input);

	/// <summary>
	/// ���̃V�[�����瑼�V�[���֐؂�ւ�鎞�̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void ChangeSceneUpdate(const Input& input);

	/// <summary>
	/// �t�F�[�h�C�����I��������̏���
	/// </summary>
	void EndFadeIn()override final;

	/// <summary>
	/// �t�F�[�h�A�E�g���I��������̏���
	/// </summary>
	void EndFadeOut()override final;

	using UpdateFunc = void (GameplayingScene::*)(const Input& input);
	UpdateFunc updater_;

	/// <summary>
	/// �Q�[�����ʏ�̕`�揈��
	/// </summary>
	void NormalDraw();

	/// <summary>
	/// �t�F�[�h�C�����̕`�揈��
	/// </summary>
	void FadeInDraw();

	/// <summary>
	/// �t�F�[�h�C�����̕`�揈��
	/// </summary>
	void FadeOutDraw();

	void UpdateVibration();

	void UpdateHUD();

	int vibrationCount_ = 0;

	using DrawFunc = void (GameplayingScene::*)();
	DrawFunc drawer_;

	uint32_t nowCount_ = 0;

	const Size vpSize_;

	std::shared_ptr<Player> player_;

	std::shared_ptr<Stage> stage_;

	std::array<float, 300> waveform_;
	int waveformIndex_ = 0;
	float nowWaveformRate_ = 0.0f;

	int candleHUDHandle_ = -1;
	float candleHUDAngle_ = 0.0f;
};