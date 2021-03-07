#pragma once
#include <array>
#include <unordered_map>
#include <string>

// ���͏���ۑ�����o�b�t�@�T�C�Y
constexpr int INPUT_RECORD_SIZE = 2;

// ���̓f�o�C�X�̎��
enum class PeripheralType
{
	keyboard,
	gamepad
};

struct PeripheralInfo
{
	PeripheralType type;
	int index;
};

enum class AnalogInputType
{
	LEFT,
	RIGHT
};

struct AnalogInputData
{
	AnalogInputData():
	horizontal(0.0f), vertical(0.0f) {}

	AnalogInputData(float hori, float vert) :
		horizontal(hori), vertical(vert) {}

	float Length()
	{
		return sqrtf(horizontal * horizontal + vertical * vertical);
	};

	float horizontal;
	float vertical;
};

class KeyConfigScene;
class GameplayingScene;

class Input
{
	friend KeyConfigScene;
	friend GameplayingScene;
public:
	Input();

	void Update();

	/// <summary>
	/// ���݉�����Ă��邩�𒲂ׂĎ擾
	/// </summary>
	/// <param name="eventName">���O</param>
	/// <param name="keyState">�L�[�̓��͏��</param>
	/// <param name="padState">�p�b�h�̓��͏��</param>
	/// <returns>true : ������Ă���</returns>
	bool CheckPressed(const char* eventName, const char* keyState, int padState);

	/// <summary>
	/// ������Ă��邩
	/// </summary>
	/// <param name="cmd">���O</param>
	/// <returns>true : ������Ă���</returns>
	bool IsPressed(const char* cmd)const;

	/// <summary>
	/// �����ꂽ�u�Ԃ�
	/// </summary>
	/// <param name="cmd">���O</param>
	/// <returns>true : ���̃t���[���ɉ����ꂽ</returns>
	bool IsTriggered(const char* cmd)const;

	/// <summary>
	/// �����ꂽ�u�Ԃ�
	/// </summary>
	/// <param name="cmd">���O</param>
	/// <returns>true : ���̃t���[���ɗ����ꂽ</returns>
	bool IsReleased(const char* cmd)const;

	AnalogInputData GetAnalogInput(AnalogInputType type)const;

private:
	/// <summary>
	/// ���t���[���̓��̓o�b�t�@�C���f�b�N�X��Ԃ�
	/// </summary>
	/// <returns>���t���[���̓��̓o�b�t�@�C���f�b�N�X</returns>
	int GetNextInputBufferIndex();

	/// <summary>
	/// �O�t���[���̓��̓o�b�t�@�C���f�b�N�X��Ԃ�
	/// </summary>
	/// <returns>�O�t���[���̓��̓o�b�t�@�C���f�b�N�X</returns>
	int GetPrevInputBufferIndex()const;

	/// <summary>
	/// ���݂̓��͏��ւ̎Q�Ƃ�Ԃ�
	/// </summary>
	/// <param name="cmd">���͏����������R�}���h�̖��O</param>
	/// <returns>true : ���݉�����Ă���</returns>
	bool& CurrentInput(const std::string cmd);

	/// <summary>
	/// ���͏��̍X�V
	/// </summary>
	void UpdateCurrentInput();

	/// <summary>
	/// ���݂̓��͏���Ԃ�
	/// </summary>
	/// <param name="cmd">���͏����������R�}���h�̖��O</param>
	/// <returns>true : ���݉�����Ă���</returns>
	bool GetCurrentInput(const std::string cmd)const;

	/// <summary>
	/// �O�t���[���̓��͏���Ԃ�
	/// </summary>
	/// <param name="cmd">���͏����������R�}���h�̖��O</param>
	/// <returns>true : ���O�ɉ�����Ă���</returns>
	bool PrevInput(const std::string cmd)const;

	/// <summary>
	/// ���͂̃}�b�s���O��ݒ�
	/// </summary>
	/// <param name="eventName">�C�ӂ̖��O</param>
	/// <param name="peri">�f�o�C�X�̎�ނȂǂ̃f�[�^</param>
	void RegistAcceptPeripheral(const char* eventName, const std::vector<PeripheralInfo>& peri);

	/// <summary>
	/// ���L�[�{�[�h���������Ă���
	/// </summary>
	/// <returns>���L�[�{�[�h���</returns>
	const std::vector<char>& GetRawKeyboardState()const;

	/// <summary>
	/// ���p�b�h���������Ă���
	/// ������1��
	/// </summary>
	/// <returns>���p�b�h���</returns>
	const int GetRawPadState()const;

	// ���݂̓��͏��o�b�t�@�̃C���f�b�N�X
	int currentInputIndex_ = 0;

	// ���݂̓��͏��o�b�t�@
	using InputStateTalbe = std::unordered_map<std::string, bool>;
	std::array<InputStateTalbe, INPUT_RECORD_SIZE> inputStateTable_;

	using PeripheralReferenceTable_t = std::unordered_map<std::string, std::vector<PeripheralInfo>>;

	/// <summary>
	/// �O��������̓e�[�u����������p
	/// </summary>
	/// <param name="prt">�ύX����e�[�u��</param>
	void SetPeripheralReferenceTable(const PeripheralReferenceTable_t& prt)const;

	mutable PeripheralReferenceTable_t peripheralReferenceTable_;
	mutable PeripheralReferenceTable_t defaultPeripheralReferenceTable_;

	mutable std::vector<char> rawKeyState_;
	mutable int rawPadState_;
};

class InputListener
{
public:
	virtual void Notify(const Input& input) = 0;
};