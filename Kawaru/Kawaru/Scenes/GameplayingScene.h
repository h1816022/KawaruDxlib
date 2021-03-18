#pragma once
#include <memory>
#include <vector>
#include "Scene.h"
#include "../Geometry.h"

class TitleScene;
class Stage;

struct Cube
{
	VECTOR center;
	VECTOR radius;
};

class GameplayingScene :
	public Scene
{
	friend SceneController;
	friend TitleScene;

public:
	~GameplayingScene();

	void Update(const Input& input)override final;

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

	using DrawFunc = void (GameplayingScene::*)();
	DrawFunc drawer_;

	uint32_t nowCount_ = 0;

	const Size vpSize_;

	Cube testCubes[2];
};