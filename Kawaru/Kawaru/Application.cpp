#include <DxLib.h>
#include "Application.h"
#include "Scenes/SceneController.h"
#include "Geometry.h"
#include "Systems/Input.h"

namespace
{
	// ��ʃT�C�Y
	constexpr int VIEWPORT_SIZE_X = 1280;
	constexpr int VIEWPORT_SIZE_Y = 720;
}

Application::Application() = default;
Application::~Application() = default;

void Application::Exit()
{
	isExit = true;
}

const Viewport& Application::GetViewport() const
{
	return viewport_;
}

Size Viewport::GetSize()const
{
	Size ret = { VIEWPORT_SIZE_X, VIEWPORT_SIZE_Y };
	return ret;
}

bool Application::Initialize()
{
	const auto& wSize = viewport_.GetSize();
	SetGraphMode(wSize.w, wSize.h, 32);
	ChangeWindowMode(true);
	SetWindowText(L"Kawaru");

	if (DxLib_Init())
	{
		return false;
	}

	sceneController_.reset(new SceneController);

	InitVKeyName();

	return true;
}

void Application::Run()
{
	Input input;

	while (ProcessMessage() == 0 && !isExit)
	{
		input.Update();

		if (input.IsTriggered("Exit"))
		{
			return;
		}

		sceneController_->Update(input);
		
		ClearDrawScreen();

		sceneController_->Draw();

		ScreenFlip();
	}
}

void Application::Terminate()
{
	sceneController_->ClearScenes();

	DxLib_End();
}

void Application::InitVKeyName()
{
	vKeyName_[KEY_INPUT_RETURN] = L"ENTER";
	vKeyName_[KEY_INPUT_ESCAPE] = L"ESC";
	vKeyName_[KEY_INPUT_0] = L"�O";
	vKeyName_[KEY_INPUT_1] = L"�P";
	vKeyName_[KEY_INPUT_2] = L"�Q";
	vKeyName_[KEY_INPUT_3] = L"�R";
	vKeyName_[KEY_INPUT_4] = L"�S";
	vKeyName_[KEY_INPUT_5] = L"�T";
	vKeyName_[KEY_INPUT_6] = L"�U";
	vKeyName_[KEY_INPUT_7] = L"�V";
	vKeyName_[KEY_INPUT_8] = L"�W";
	vKeyName_[KEY_INPUT_9] = L"�X";
	vKeyName_[KEY_INPUT_A] = L"�`";
	vKeyName_[KEY_INPUT_B] = L"�a";
	vKeyName_[KEY_INPUT_C] = L"�b";
	vKeyName_[KEY_INPUT_D] = L"�c";
	vKeyName_[KEY_INPUT_E] = L"�d";
	vKeyName_[KEY_INPUT_F] = L"�e";
	vKeyName_[KEY_INPUT_G] = L"�f";
	vKeyName_[KEY_INPUT_H] = L"�g";
	vKeyName_[KEY_INPUT_I] = L"�h";
	vKeyName_[KEY_INPUT_J] = L"�i";
	vKeyName_[KEY_INPUT_K] = L"�j";
	vKeyName_[KEY_INPUT_L] = L"�k";
	vKeyName_[KEY_INPUT_M] = L"�l";
	vKeyName_[KEY_INPUT_N] = L"�m";
	vKeyName_[KEY_INPUT_O] = L"�n";
	vKeyName_[KEY_INPUT_P] = L"�o";
	vKeyName_[KEY_INPUT_Q] = L"�p";
	vKeyName_[KEY_INPUT_R] = L"�q";
	vKeyName_[KEY_INPUT_S] = L"�r";
	vKeyName_[KEY_INPUT_T] = L"�s";
	vKeyName_[KEY_INPUT_U] = L"�t";
	vKeyName_[KEY_INPUT_V] = L"�u";
	vKeyName_[KEY_INPUT_W] = L"�v";
	vKeyName_[KEY_INPUT_X] = L"�w";
	vKeyName_[KEY_INPUT_Y] = L"�x";
	vKeyName_[KEY_INPUT_Z] = L"�y";
	vKeyName_[KEY_INPUT_BACK] = L"BackSpace";
	vKeyName_[KEY_INPUT_TAB] = L"Tab";
	vKeyName_[KEY_INPUT_LSHIFT] = L"��Shift";
	vKeyName_[KEY_INPUT_RSHIFT] = L"�EShift";
	vKeyName_[KEY_INPUT_LCONTROL] = L"��Ctrl";
	vKeyName_[KEY_INPUT_RCONTROL] = L"�ECtrl";
	vKeyName_[KEY_INPUT_ESCAPE] = L"Esc";
	vKeyName_[KEY_INPUT_SPACE] = L"�X�y�[�X";
	vKeyName_[KEY_INPUT_PGUP] = L"PageUp";
	vKeyName_[KEY_INPUT_PGDN] = L"PageDown";
	vKeyName_[KEY_INPUT_END] = L"End";
	vKeyName_[KEY_INPUT_HOME] = L"Home";
	vKeyName_[KEY_INPUT_LEFT] = L"��";
	vKeyName_[KEY_INPUT_UP] = L"��";
	vKeyName_[KEY_INPUT_RIGHT] = L"�E";
	vKeyName_[KEY_INPUT_DOWN] = L"��";
	vKeyName_[KEY_INPUT_INSERT] = L"Insert";
	vKeyName_[KEY_INPUT_DELETE] = L"Delete";
	vKeyName_[KEY_INPUT_F1] = L"�e�P";
	vKeyName_[KEY_INPUT_F2] = L"�e�Q";
	vKeyName_[KEY_INPUT_F3] = L"�e�R";
	vKeyName_[KEY_INPUT_F4] = L"�e�S";
	vKeyName_[KEY_INPUT_F5] = L"�e�T";
	vKeyName_[KEY_INPUT_F6] = L"�e�U";
	vKeyName_[KEY_INPUT_F7] = L"�e�V";
	vKeyName_[KEY_INPUT_F8] = L"�e�W";
	vKeyName_[KEY_INPUT_F9] = L"�e�X";
	vKeyName_[KEY_INPUT_F10] = L"�e�P�O";
	vKeyName_[KEY_INPUT_F11] = L"�e�P�P";
	vKeyName_[KEY_INPUT_F12] = L"�e�P�Q";
}

const wchar_t* Application::GetVKeyName(int index)
{
	return vKeyName_[index];
}

const std::unordered_map<int, const wchar_t*>& Application::GetAllVKeyName() const
{
	return vKeyName_;
}