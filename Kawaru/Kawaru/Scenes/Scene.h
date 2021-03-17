#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include "../Geometry.h"

class SceneController;
class Input;
class Actor;

enum class FADE_MODE
{
	Non,
	In,
	Out
};

namespace
{
	// �t�F�[�h�ɂ����鎞��
	constexpr uint32_t FADE_INTERVAL = 60;

	// ��ʘg�̑傫��
	constexpr int LETTERBOX_MASK_LENGTH = 50;

	// �V�[���̕\�����d�˂�Ƃ��̃}�X�N�̓����x
	constexpr int OVERLAP_TRANSPARENCY = 120;

	// ���j���[�̓����x
	constexpr int MENU_TRANSPARENCY = 200;
}

class Scene
{
	friend SceneController;

public:
	virtual ~Scene();

	virtual void Update(const Input& input) {};
	
	virtual void Draw() {};

	Scene(const Scene& scene) = default;

	/// <summary>
	/// �t�F�[�h�C���I�����̏���
	/// </summary>
	virtual void EndFadeIn() {};

	/// <summary>
	///  �t�F�[�h�A�E�g�I�����̏���
	/// </summary>
	virtual void EndFadeOut() {};

	void AddActors(std::shared_ptr<Actor> actor);

	std::vector<std::shared_ptr<Actor>> GetActors(const std::wstring& tag);

	//void DestroyActor(std::shared_ptr<Actor> actor);

protected:
	Scene(SceneController& controller);

	void UpdateFade(const Input& input);

	/// <summary>
	/// �t�F�[�h���J�n����
	/// </summary>
	/// <param name="mode">�C�� / �A�E�g���w��</param>
	void StartFade(const FADE_MODE& mode);

	/// <summary>
	/// �l�X�ȕ`��̌�̉�ʌ���(�t�F�[�h�Ȃ�)��`��
	/// </summary>
	void PostDraw();

	/// <summary>
	/// �e�t���̕������`��
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="text">�`�悷�镶����</param>
	/// <param name="color">�����F</param>
	void DrawDropShadowString(const Position2& pos, const wchar_t* text, unsigned int color = 0xffffff);

	void UpdateActors(const Input& input);
	void DrawActors();

	SceneController& controller_;

private:
	/// <summary>
	/// �t�F�[�h����
	/// </summary>
	/// <param name="rate">�Z�x</param>
	void Fade(float rate);

	FADE_MODE fadeMode_;

	int fadeCount_;

	float nowFadeRate = 0.0f;

	std::vector<std::shared_ptr<Actor>> actors_;
};