#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include "../Geometry.h"

class SceneController;
class Input;
class Actor;

// �t�F�[�h�̎��
enum class FADE_MODE
{
	Non,	// �t�F�[�h�Ȃ�
	In,		// ���񂾂񐰂��
	Out		// ���񂾂�^���Â�
};

namespace
{
	// ��{�̃t�F�[�h�ɂ����鎞��
	constexpr uint32_t DEFAULT_FADE_INTERVAL = 60;

	// ��ʘg�̑傫��
	constexpr int LETTERBOX_MASK_LENGTH = 50;

	// �V�[���̕\�����d�˂�Ƃ��̃}�X�N�̓����x
	constexpr int OVERLAP_TRANSPARENCY = 120;

	// ���j���[�̓����x
	constexpr int MENU_TRANSPARENCY = 200;
}

/// <summary>
/// �V�[�����N���X
/// </summary>
class Scene
{
	friend SceneController;

public:
	virtual ~Scene();

	/// <summary>
	///  �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	virtual void Update(const Input& input) {};
	
	/// <summary>
	/// �`�揈��
	/// </summary>
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

	/// <summary>
	/// �A�N�^�[�̒ǉ��o�^
	/// </summary>
	/// <param name="actor">�o�^����A�N�^�[</param>
	void AddActors(std::shared_ptr<Actor> actor);

	/// <summary>
	/// �o�^����Ă���A�N�^�[����A�w��^�O�ɊY��������̂�S�Ď擾
	/// </summary>
	/// <param name="tag">�擾�������A�N�^�[�̃^�O</param>
	/// <returns>�w��^�O�̃A�N�^�[�̃��X�g</returns>
	std::vector<std::shared_ptr<Actor>> GetActors(const std::wstring& tag);

protected:
	Scene(SceneController& controller);

	/// <summary>
	/// �t�F�[�h�����̍X�V
	/// </summary>
	/// <param name="input">���͏��</param>
	void UpdateFade(const Input& input);

	/// <summary>
	/// �t�F�[�h���J�n����
	/// </summary>
	/// <param name="mode">�C�� / �A�E�g���w��</param>
	/// <param name="time">�t�F�[�h�Ɋ|���鎞��</param>
	void StartFade(const FADE_MODE& mode, int time = DEFAULT_FADE_INTERVAL);

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

	/// <summary>
	/// �o�^�A�N�^�[�B�̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void UpdateActors(const Input& input);

	/// <summary>
	/// �o�^�A�N�^�[�B�̕`�揈��
	/// </summary>
	void DrawActors();

	SceneController& controller_;

private:
	/// <summary>
	/// �t�F�[�h����
	/// </summary>
	/// <param name="rate">�Z�x</param>
	void Fade(float rate);

	FADE_MODE fadeMode_;

	int nowFadeCount_;

	int fadeInterval_;

	float nowFadeRate = 0.0f;

	std::vector<std::shared_ptr<Actor>> actors_;
};