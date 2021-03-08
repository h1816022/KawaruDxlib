#include "Camera.h"
#include "../Systems/Input.h"
#include "Stage.h"

namespace
{
	// ��]���x
	constexpr float ROT_SPEED = 0.05f;

	// �����_�ɐݒ肵���A�N�^�[����̍����̃I�t�Z�b�g�A��l�̎��_��
	constexpr float TARGET_HEIGHT_OFFSET_FIRST = 800.0f;

	// �����_�ɐݒ肵���A�N�^�[����̍����̃I�t�Z�b�g�A�O�l�̎��_��
	constexpr float TARGET_HEIGHT_OFFSET_THIRD = 400.0f;

	// ���_���W���璍���_�܂ł̋����A��l�̎��_��
	constexpr float CAMERA_ARM_LENGTH_FIRST = 1.0f;

	// ���_���W���璍���_�܂ł̋����A�O�l�̎��_��
	constexpr float CAMERA_ARM_LENGTH_THIRD = 1600.0f;

	// �Օ������o�p�R���W�����̑傫��
	constexpr float COLLISION_SIZE = 50.0f;
}

Camera::Camera(const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(posX, posY, posZ), 
	stage_(stage), 
	targetHeightOffset_(TARGET_HEIGHT_OFFSET_THIRD),
	armLength_(CAMERA_ARM_LENGTH_THIRD)
{
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(false);

	angleH_ = DX_PI_F;
	angleV_ = 0.0f;
}

Camera::~Camera()
{
}

void Camera::Update(const Input& input)
{
	UpdateAngle(input);

	targetPos_ = VAdd(targetActor_->GetPos(), VGet(0.0f, targetHeightOffset_, 0.0f));

	if (input.IsPressed("Camera"))
	{
		UpdatePos();
	}

	// �J�����̏������C�u�����̃J�����ɔ��f������
	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));
	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

void Camera::Draw()
{
	for (auto& targetPos : player_->GetLineTraceSamplingOffsets())
	{
		DrawSphere3D(VAdd(player_->GetPos(), targetPos), 50.0f, 100, 0xffff00, 0xffffff, true);
	}

	
	if (CanSeeThePlayer()
		)
	{
		DrawString(0, 0, L"true", 0xffffff);
	}
	else
	{
		DrawString(0, 0, L"false", 0xffffff);
	}
}

const VECTOR& Camera::GetTargetPos() const
{
	return targetPos_;
}

void Camera::SetTargetActor(std::shared_ptr<Actor> target)
{
	targetActor_ = target;
}

void Camera::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
}

bool Camera::CanSeeThePlayer()
{
	for (auto& targetPos : player_->GetLineTraceSamplingOffsets())
	{
		auto hitResult = stage_.CollCheckLine(pos_, VAdd(player_->GetPos(), targetPos));
		if (!hitResult.HitFlag)
		{
			return true;
		}
	}

	return false;
}

void Camera::UpdateAngle(const Input& input)
{
	auto analogInpoutData = input.GetAnalogInput(AnalogInputType::RIGHT);

	if (analogInpoutData.horizontal != 0.0f)
	{
		angleH_ += ROT_SPEED * analogInpoutData.horizontal;
	}

	if (analogInpoutData.vertical != 0.0f)
	{
		angleV_ += ROT_SPEED * analogInpoutData.vertical;
	}

	ClampAngle();
}

void Camera::UpdatePos()
{
	MATRIX rotZ;
	MATRIX rotY;
	MV1_COLL_RESULT_POLY_DIM hitResult;
	int hitNum;

	rotY = MGetRotY(angleH_);
	rotZ = MGetRotZ(angleV_);

	// �J�����̍��W���Z�o
	pos_ = VAdd(VTransform(VTransform(VGet(-armLength_, 0.0f, 0.0f), rotZ), rotY), targetPos_);

	// �����_����J�����̍��W�܂ł̊ԂɃX�e�[�W�̃|���S�������邩���ׂ�
	hitResult = stage_.CollCheckCapsule(targetPos_, pos_, COLLISION_SIZE);
	hitNum = hitResult.HitNum;
	MV1CollResultPolyDimTerminate(hitResult);
	if (hitNum != 0)
	{
		float notHitLength;
		float hitLength;
		float testLength;
		VECTOR testPosition;

		// ������Ȃ��ʒu�܂Ńv���C���[�ɋ߂Â�

		notHitLength = 0.0f;

		hitLength = armLength_;
		do
		{
			testLength = notHitLength + (hitLength - notHitLength) / 2.0f;

			testPosition = VAdd(VTransform(VTransform(VGet(-testLength, 0.0f, 0.0f), rotZ), rotY), targetPos_);

			hitResult = stage_.CollCheckCapsule(targetPos_, testPosition, COLLISION_SIZE);
			hitNum = hitResult.HitNum;
			MV1CollResultPolyDimTerminate(hitResult);
			if (hitNum != 0)
			{
				hitLength = testLength;
			}
			else
			{
				notHitLength = testLength;
			}

		} while (hitLength - notHitLength > 0.1f);

		pos_ = testPosition;
	}
}

void Camera::ClampAngle()
{
	if (angleH_ < -DX_PI_F)
	{
		angleH_ += DX_TWO_PI_F;
	}
	else if (angleH_ > DX_PI_F)
	{
		angleH_ -= DX_TWO_PI_F;
	}

	if (angleV_ < -DX_PI_F / 2.0f + 0.6f)
	{
		angleV_ = -DX_PI_F / 2.0f + 0.6f;
	}
	else if (angleV_ > DX_PI_F / 2.0f - 0.6f)
	{
		angleV_ = DX_PI_F / 2.0f - 0.6f;
	}
}