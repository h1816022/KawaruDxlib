#pragma once
#include <vector>
#include <functional>
#include "Scene.h"

// ���j���[���ڂ��Ƃ̃f�[�^
struct MenuItem
{
	bool isActive = false;
	std::wstring menuText;
	Position2 pos;

	// ���s���鏈��
	std::function<void(void)> func;

	MenuItem(const wchar_t* str,
		const Position2& p,
		std::function<void(void)> f)
		:menuText(str),
		func(f),
		pos(p)
	{}
};

namespace
{
	// ���ɐݒ�̂Ȃ����́A�I�𒆂łȂ����j���[���ڂ�X�����W�I�t�Z�b�g
	constexpr int DEFAULT_MENU_OFFSET_X = 64;

	// ���ɐݒ�̂Ȃ����́A���j���[���ڂ̍ŏ���Y�����W�I�t�Z�b�g
	constexpr int DEFAULT_MENU_START_OFFSET_Y = 96;

	// ���ɐݒ�̂Ȃ����́A���j���[���ڂ̊Ԃ�Y�����W�I�t�Z�b�g
	constexpr int DEFAULT_MENU_INTERVAL_OFFSET_Y = 48;

	// ���j���[���ڂ̊�{�F
	constexpr unsigned int DEFAULT_MENU_COLOR = 0xcccccc;

	// �J�[�\�����d�Ȃ��Ă���Ƃ��̃��j���[���ڂ̐F
	constexpr unsigned int OVERLAP_MENU_COLOR = 0xffff77;

	// �I�����ꂽ���j���[���ڂ̐F
	constexpr unsigned int CHOICE_MENU_COLOR = 0xff3333;
}

// ���j���[���ڂ̕\���I�t�Z�b�g
struct MenuOffsets
{
	MenuOffsets(const int x = DEFAULT_MENU_OFFSET_X, const int startY = DEFAULT_MENU_START_OFFSET_Y, const int intervalY = DEFAULT_MENU_INTERVAL_OFFSET_Y) :
		menuOffsetX(x),
		menuStartOffsetY(startY),
		menuIntervalOffsetY(intervalY)
	{};

	// �I�𒆂łȂ����j���[���ڂ�X�����W�I�t�Z�b�g
	const int menuOffsetX;

	// ���j���[���ڂ̍ŏ���Y�����W�I�t�Z�b�g
	const int menuStartOffsetY;

	// ���j���[���ڂ̊Ԃ�Y�����W�I�t�Z�b�g
	const int menuIntervalOffsetY;
};

/// <summary>
/// ���j���[���ڂ�\��������V�[���̊��N���X
/// </summary>
class MenuSceneBase :
	public Scene
{
public:
	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void Update(const Input& input)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw()override;

protected:
	/// <param name="controller">�V�[���̃R���g���[���[</param>
	/// <param name="blankStart">�ŏ��ɃJ�[�\�����B���āA�I��Ō��肷��܂łɂЂƑ���K�v�ɂ��邩</param>
	/// <param name="offsets">���j���[���ڂ̐F�X�ȃI�t�Z�b�g�l</param>
	MenuSceneBase(SceneController& controller,
		bool blankStart = false,
		const MenuOffsets& offsets = MenuOffsets()
	);

	/// <summary>
	/// ���j���[���ڂ�ݒ�
	/// </summary>
	/// <param name="text">�\���e�L�X�g</param>
	/// <param name="func">�I�������Ƃ��̏���</param>
	void InitMenuItems(const wchar_t* text, std::function<void(void)> func);

	/// <summary>
	/// �ݒ肵�����j���[���ڂ����ƂɃJ�[�\���Ȃǂ̏����ݒ������
	/// </summary>
	void InitMenu();

	/// <summary>
	/// �ݒ肵�����j���[���ڂ����ƂɃJ�[�\���Ȃǂ̏����ݒ������
	/// </summary>
	/// <param name="menuFrame">���j���[�̘g</param>
	void InitMenu(const Rect menuFrame);

	/// <summary>
	/// �Ăяo�����̃V�[���֖߂�
	/// </summary>
	virtual void CloseMenu();

	/// <summary>
	/// ���̃��j���[�V�[���𐶐�
	/// </summary>
	/// <param name="newScene">���̃��j���[�V�[��</param>
	void MakeNextMenu(MenuSceneBase* newScene);

	// ����
	std::vector<MenuItem> menuItems_;

	// ���ݑI�΂�Ă��鍀�ڔԍ�
	int currentSelectNo_ = 0;

	// ���j���[�^�C�g��
	std::wstring title_ = L"�^�C�g��������";

	// ���͉\��
	bool canInput_ = true;

	// ���j���[�\���̃Y����
	Vector2 offset_ = { 0, 0 };

	// �I�΂ꂽ���j���[���ڂ̐F
	unsigned int choiceColor_;

private:
	/// <summary>
	/// �J�[�\���𓮂��n�߂�����
	/// </summary>
	/// <param name="isUp">true : �����, false ; ������</param>
	void CursorMoveStart(bool isUp);

	/// <summary>
	/// �J�[�\���̓������X�V
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// �J�[�\���̕`��
	/// </summary>
	void DrawCursor();

	/// <summary>
	/// �g�̕`��
	/// </summary>
	void DrawFrame();

	/// <summary>
	/// ���j���[���ڂ̕`��
	/// </summary>
	void DrawMenu(bool isBlank);

	/// <summary>
	/// �ЂƓ��͑҂��Ă���Ƃ��̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void BlankUpdate(const Input& input);

	/// <summary>
	/// �ʏ�̍X�V����
	/// </summary>
	/// <param name="input">���͏��</param>
	void NormalUpdate(const Input& input);

	using Update_t = void (MenuSceneBase::*)(const Input& input);
	Update_t updater_;

	/// <summary>
	/// �ЂƓ��͑҂��Ă���Ƃ��̕`�揈��
	/// </summary>
	void BlankDraw();

	/// <summary>
	/// �ʏ�̕`�揈��
	/// </summary>
	void NormalDraw();

	using Draw_t = void (MenuSceneBase::*)();
	Draw_t drawer_;

	Vector2 cursorSize_;

	int cursorH_;

	int32_t cursorCount_;

	int cursorRotaTime_;

	// �J�[�\����������Ɉړ����Ă���
	bool isUp_ = false;

	// ���������������Ă���
	bool isInitialized = false;

	// ���݂̃J�[�\���̍��W
	Position2 nowCursorPos_;

	// �J�[�\���̖ڕW�n�_�̍��W
	Position2 targetCursorPos_;

	// ���݂̃J�[�\���̉�]��
	double nowCursorRotaAmount_;

	// ���j���[�̘g
	Rect menuFrame_;

	// ���j���[���ڂ�X�I�t�Z�b�g�l
	int menuOffsetX_ = 0;

	// ���j���[���ڂ�Y�I�t�Z�b�g�l
	int menuOffsetY_ = 0;

	// ���j���[���ڊԂ̃I�t�Z�b�g�l
	int menuIntervalOffsetY_;
};