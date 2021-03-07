#include <DxLib.h>
#include "Application.h"
#include "Scenes/SceneController.h"
#include "Geometry.h"
#include "Systems/Input.h"

namespace
{
	// âÊñ ÉTÉCÉY
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
	vKeyName_[KEY_INPUT_0] = L"ÇO";
	vKeyName_[KEY_INPUT_1] = L"ÇP";
	vKeyName_[KEY_INPUT_2] = L"ÇQ";
	vKeyName_[KEY_INPUT_3] = L"ÇR";
	vKeyName_[KEY_INPUT_4] = L"ÇS";
	vKeyName_[KEY_INPUT_5] = L"ÇT";
	vKeyName_[KEY_INPUT_6] = L"ÇU";
	vKeyName_[KEY_INPUT_7] = L"ÇV";
	vKeyName_[KEY_INPUT_8] = L"ÇW";
	vKeyName_[KEY_INPUT_9] = L"ÇX";
	vKeyName_[KEY_INPUT_A] = L"Ç`";
	vKeyName_[KEY_INPUT_B] = L"Ça";
	vKeyName_[KEY_INPUT_C] = L"Çb";
	vKeyName_[KEY_INPUT_D] = L"Çc";
	vKeyName_[KEY_INPUT_E] = L"Çd";
	vKeyName_[KEY_INPUT_F] = L"Çe";
	vKeyName_[KEY_INPUT_G] = L"Çf";
	vKeyName_[KEY_INPUT_H] = L"Çg";
	vKeyName_[KEY_INPUT_I] = L"Çh";
	vKeyName_[KEY_INPUT_J] = L"Çi";
	vKeyName_[KEY_INPUT_K] = L"Çj";
	vKeyName_[KEY_INPUT_L] = L"Çk";
	vKeyName_[KEY_INPUT_M] = L"Çl";
	vKeyName_[KEY_INPUT_N] = L"Çm";
	vKeyName_[KEY_INPUT_O] = L"Çn";
	vKeyName_[KEY_INPUT_P] = L"Ço";
	vKeyName_[KEY_INPUT_Q] = L"Çp";
	vKeyName_[KEY_INPUT_R] = L"Çq";
	vKeyName_[KEY_INPUT_S] = L"Çr";
	vKeyName_[KEY_INPUT_T] = L"Çs";
	vKeyName_[KEY_INPUT_U] = L"Çt";
	vKeyName_[KEY_INPUT_V] = L"Çu";
	vKeyName_[KEY_INPUT_W] = L"Çv";
	vKeyName_[KEY_INPUT_X] = L"Çw";
	vKeyName_[KEY_INPUT_Y] = L"Çx";
	vKeyName_[KEY_INPUT_Z] = L"Çy";
	vKeyName_[KEY_INPUT_BACK] = L"BackSpace";
	vKeyName_[KEY_INPUT_TAB] = L"Tab";
	vKeyName_[KEY_INPUT_LSHIFT] = L"ç∂Shift";
	vKeyName_[KEY_INPUT_RSHIFT] = L"âEShift";
	vKeyName_[KEY_INPUT_LCONTROL] = L"ç∂Ctrl";
	vKeyName_[KEY_INPUT_RCONTROL] = L"âECtrl";
	vKeyName_[KEY_INPUT_ESCAPE] = L"Esc";
	vKeyName_[KEY_INPUT_SPACE] = L"ÉXÉyÅ[ÉX";
	vKeyName_[KEY_INPUT_PGUP] = L"PageUp";
	vKeyName_[KEY_INPUT_PGDN] = L"PageDown";
	vKeyName_[KEY_INPUT_END] = L"End";
	vKeyName_[KEY_INPUT_HOME] = L"Home";
	vKeyName_[KEY_INPUT_LEFT] = L"ç∂";
	vKeyName_[KEY_INPUT_UP] = L"è„";
	vKeyName_[KEY_INPUT_RIGHT] = L"âE";
	vKeyName_[KEY_INPUT_DOWN] = L"â∫";
	vKeyName_[KEY_INPUT_INSERT] = L"Insert";
	vKeyName_[KEY_INPUT_DELETE] = L"Delete";
	vKeyName_[KEY_INPUT_F1] = L"ÇeÇP";
	vKeyName_[KEY_INPUT_F2] = L"ÇeÇQ";
	vKeyName_[KEY_INPUT_F3] = L"ÇeÇR";
	vKeyName_[KEY_INPUT_F4] = L"ÇeÇS";
	vKeyName_[KEY_INPUT_F5] = L"ÇeÇT";
	vKeyName_[KEY_INPUT_F6] = L"ÇeÇU";
	vKeyName_[KEY_INPUT_F7] = L"ÇeÇV";
	vKeyName_[KEY_INPUT_F8] = L"ÇeÇW";
	vKeyName_[KEY_INPUT_F9] = L"ÇeÇX";
	vKeyName_[KEY_INPUT_F10] = L"ÇeÇPÇO";
	vKeyName_[KEY_INPUT_F11] = L"ÇeÇPÇP";
	vKeyName_[KEY_INPUT_F12] = L"ÇeÇPÇQ";
}

const wchar_t* Application::GetVKeyName(int index)
{
	return vKeyName_[index];
}

const std::unordered_map<int, const wchar_t*>& Application::GetAllVKeyName() const
{
	return vKeyName_;
}