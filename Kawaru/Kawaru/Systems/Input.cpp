#include <DxLib.h>
#include <cassert>
#include "Input.h"
#include "../Tools/Arithmetic.h"

namespace
{
	// パッドの入力を割り当てたくないときに使うダミーマスク値
	constexpr int PAD_INPUT_INVALID = 0;

	// 入力ありとするかどうかのしきい値
	constexpr float ANALOG_INPUT_THRESHOLD = 0.1f;
}

Input::Input()
{
	rawKeyState_.resize(256);

	currentInputIndex_ = 0;
	RegistAcceptPeripheral("OK", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_RETURN }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_R} });
	RegistAcceptPeripheral("Pause", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_P }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_L} });
	
	RegistAcceptPeripheral("LUp", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_W }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("LDown", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_S }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("LLeft", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_A }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("LRight", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_D }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("RUp", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_UP }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("RDown", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_DOWN }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("RLeft", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_LEFT }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });
	RegistAcceptPeripheral("RRight", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_RIGHT }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_INVALID} });

	RegistAcceptPeripheral("MenuUp", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_W }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_UP} });
	RegistAcceptPeripheral("MenuDown", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_S }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_DOWN} });
	
	RegistAcceptPeripheral("Exit", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_ESCAPE }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_Z} });
	RegistAcceptPeripheral("Call", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_C }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_Y} });
	RegistAcceptPeripheral("Jump", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_SPACE }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_A} });
	RegistAcceptPeripheral("Camera", { { PERIPHERAL_TYPE::Keyboard, KEY_INPUT_LSHIFT }, { PERIPHERAL_TYPE::Gamepad, PAD_INPUT_C} });
}

void Input::Update()
{
	currentInputIndex_ = GetNextInputBufferIndex();

	UpdateCurrentInput();
}

bool Input::IsPressed(const char* cmd) const
{
	return GetCurrentInput(cmd);
}

bool Input::CheckPressed(const char* eventName, const char* keyState, int padState)
{
	bool ret = false;

	for (auto& p : peripheralReferenceTable_[eventName])
	{
		if (p.type == PERIPHERAL_TYPE::Keyboard)
		{
			ret = ret || keyState[p.index];
		}
		else if (p.type == PERIPHERAL_TYPE::Gamepad)
		{
			ret = ret || (padState & p.index);
		}
	}

	return ret;
}

bool Input::IsTriggered(const char* cmd) const
{
	return GetCurrentInput(cmd) && !PrevInput(cmd);
}

bool Input::IsReleased(const char* cmd) const
{
	return !GetCurrentInput(cmd) && PrevInput(cmd);
}

AnalogInputData Input::GetAnalogInput(ANALOG_INPUT_TYPE type)const
{
	DINPUT_JOYSTATE input;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	AnalogInputData data;

	constexpr float MAX_ANALOG_NUM = 1000.0f;

	if (type == ANALOG_INPUT_TYPE::Left)
	{
		data = { static_cast<float>(input.X) / MAX_ANALOG_NUM, static_cast<float>(input.Y) / -MAX_ANALOG_NUM };

		data.horizontal = IsPressed("LLeft") ? -1.0f : (IsPressed("LRight") ? 1.0f : data.horizontal);
		data.vertical = IsPressed("LDown") ? -1.0f : (IsPressed("LUp") ? 1.0f : data.vertical);
	}
	else
	{
		data = { static_cast<float>(input.Rx) / MAX_ANALOG_NUM, static_cast<float>(input.Ry) / -MAX_ANALOG_NUM };

		data.horizontal = IsPressed("RLeft") ? -1.0f : (IsPressed("RRight") ? 1.0f : data.horizontal);
		data.vertical = IsPressed("RDown") ? -1.0f : (IsPressed("RUp") ? 1.0f : data.vertical);
	}

	data.horizontal = abs(data.horizontal) > ANALOG_INPUT_THRESHOLD ? data.horizontal : 0.0f;
	data.vertical = abs(data.vertical) > ANALOG_INPUT_THRESHOLD ? data.vertical : 0.0f;

	return data;
}

int Input::GetNextInputBufferIndex()
{
	return ModuloIncrement(currentInputIndex_, static_cast<int>(inputStateTable_.size()));
}

int Input::GetPrevInputBufferIndex()const
{
	return ModuloDecrement(currentInputIndex_, static_cast<int>(inputStateTable_.size()));
}

bool& Input::CurrentInput(const std::string cmd)
{
	auto& currentTbl = inputStateTable_[currentInputIndex_];

	auto it = currentTbl.find(cmd);

	assert(it != currentTbl.end());

	return it->second;
}

void Input::UpdateCurrentInput()
{
	GetHitKeyStateAll(rawKeyState_.data());

	rawPadState_ = GetJoypadInputState(DX_INPUT_PAD1);

	for (auto& p : peripheralReferenceTable_)
	{
		CurrentInput(p.first) = CheckPressed(p.first.c_str(), rawKeyState_.data(), rawPadState_);
	}
}

bool Input::GetCurrentInput(const std::string cmd) const
{
	auto& currentTbl = inputStateTable_[currentInputIndex_];

	auto it = currentTbl.find(cmd);

	assert(it != currentTbl.end());

	return it->second;
}

bool Input::PrevInput(const std::string cmd)const
{
	auto lastInputIndex = GetPrevInputBufferIndex();
	auto& lastTbl = inputStateTable_[lastInputIndex];

	auto it = lastTbl.find(cmd);

	assert(it != lastTbl.end());
	return it->second;
}

void Input::RegistAcceptPeripheral(const char* eventName, const std::vector<PeripheralInfo>& peri)
{
	peripheralReferenceTable_[eventName] = peri;
	defaultPeripheralReferenceTable_[eventName] = peri;
	for (auto& t : inputStateTable_)
	{
		t[eventName] = false;
	}
}

const std::vector<char>& Input::GetRawKeyboardState() const
{
	return rawKeyState_;
}

const int Input::GetRawPadState() const
{
	return rawPadState_;
}

void Input::SetPeripheralReferenceTable(const PeripheralReferenceTable_t& prt) const
{
	for (auto& p : prt)
	{
		peripheralReferenceTable_[p.first] = p.second;
	}
}