#include <Dxlib.h>
#include <array>
#include <memory>
#include "Player.h"
#include "../Camera.h"
#include "../Stage.h"
#include "../../NavMesh/NavMesh.h"
#include "../../NavMesh/NavMeshPath.h"
#include "../../Systems/Input.h"

namespace
{
	// �ړ����x
	constexpr float DEFAULT_MOVE_SPEED = 30.0f;
		
	// �����蔻��J�v�Z���̔��a
	constexpr float HIT_WIDTH = 150.0f;

	// �����蔻��J�v�Z���̍���
	constexpr float HIT_HEIGHT = 680.0f;

	// �̂̕�
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;
}

Player::Player(const Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(/*L"Models/Hatune/�����~�N.pmd"*/L"Models/DxChara.x", L"Models/Hatune/Hatune", stage, HIT_WIDTH, HIT_HEIGHT, posX, posY, posZ), camera_(camera)
{
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));

	//MV1SetScale(modelHandle_, VGet(35.0f, 35.0f, 35.0f));

	int materialNum = MV1GetMaterialNum(modelHandle_);

	for (int i = 0; i < materialNum; ++i)
	{
		float dotWidth = MV1GetMaterialOutLineDotWidth(modelHandle_, i);

		MV1SetMaterialOutLineDotWidth(modelHandle_, i, dotWidth / 25.0f);
	}
}

Player::~Player()
{
}

void Player::Update(const Input& input)
{
	VECTOR upMoveVec;
	VECTOR leftMoveVec;

	Character::Update(input);

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	CalcUnitMoveVector(upMoveVec, leftMoveVec);

	// �e�����̃x�N�g�����A�ړ��x�N�g�����Z�o
	moveFlag_ = CalcMoveVector(moveVec_, upMoveVec, leftMoveVec, input);

	if (moveFlag_)
	{
		moveDirection_ = VNorm(moveVec_);

		const float MOVE_SPEED_RATE = input.GetAnalogInput(ANALOG_INPUT_TYPE::Left).Length();

		moveVec_ = VScale(moveDirection_, DEFAULT_MOVE_SPEED * MOVE_SPEED_RATE);
	}

	(this->*updater_)(input);

	UpdateAngle();

	UpdatePos(moveVec_);

	UpdateAnim();
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);

	DrawShadow();
}

const std::vector<VECTOR>& Player::GetLineTraceSamplingOffsets()const
{
	return lineTraceSamplingOffsets_;
}

void Player::IdleUpdate(const Input& input)
{
	if (input.IsTriggered("Jump"))
	{
		Jump();
		return;
	}

	if (moveFlag_)
	{
		ChangeAnim(ANIM_NAME::Walk);
		ChangeUpadater(UPDATE_TYPE::Run);
	}
}

void Player::RunUpdate(const Input& input)
{
	if (input.IsTriggered("Jump"))
	{
		Jump();
		return;
	}

	if (!moveFlag_)
	{
		ChangeAnim(ANIM_NAME::Idle);
		ChangeUpadater(UPDATE_TYPE::Idle);
	}
}

void Player::CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec)
{
	// �O�����
	upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
	upMoveVec.y = 0.0f;

	// ���E����
	leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

	// ��̃x�N�g���𐳋K��
	upMoveVec = VNorm(upMoveVec);
	leftMoveVec = VNorm(leftMoveVec);
}

bool Player::CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input)
{
	// ���̃t���[���ł̈ړ��x�N�g����������
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	bool moveFlag = false;

	auto analogInpoutData = input.GetAnalogInput(ANALOG_INPUT_TYPE::Left);

	// ���E�ړ�
	if (analogInpoutData.horizontal != 0.0f)
	{
		moveVec = VAdd(moveVec, VScale(leftMoveVec, -analogInpoutData.horizontal));
	
		moveFlag = true;
	}

	if (analogInpoutData.vertical != 0.0f)
	{
		moveVec = VAdd(moveVec, VScale(upMoveVec, analogInpoutData.vertical));

		moveFlag = true;
	}

	return moveFlag;
}