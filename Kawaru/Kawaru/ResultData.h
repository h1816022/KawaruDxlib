#pragma once

/// <summary>
/// ���U���g�ɕ\������A�V�[�����܂����ŋL�^�A�g�p����f�[�^
/// </summary>
struct ResultData
{
	ResultData();

	/// <summary>
	/// �f�[�^�̏�����
	/// </summary>
	void Reset();

	// �v���C���Ă�������
	unsigned int playTime = 0;

	// �S�[�X�g���Ă񂾉�
	unsigned int callCount = 0;

	// �N���A������
	bool gameClear = false;

	// �Q�[���I�[�o�[�ɂȂ�����
	bool gameOver = false;
};