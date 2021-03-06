#include "Camera.h"
#include "../Systems/Input.h"
#include "Stage.h"

namespace
{
	// ��]���x
	constexpr float ROT_SPEED = 0.05f;

	// �����_�ɐݒ肵���A�N�^�[����̍����̃I�t�Z�b�g
	constexpr float TARGET_HEIGHT_OFFSET = 400.0f;

	// ���_���W���璍���_�܂ł̋���
	constexpr float CAMERA_ARM_LENGTH = 1600.0f;

	constexpr float COLLISION_SIZE = 50.0f;
}

Camera::Camera(const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(posX, posY, posZ), stage_(stage)
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
	// �p�b�h�̂R�{�^�����A�V�t�g�L�[��������Ă���ꍇ�̂݊p�x�ύX������s��
	if (input.IsPressed("Camera"))
	{
		// �u���v�{�^����������Ă����琅���p�x���}�C�i�X����
		if (input.IsPressed("Left"))
		{
			angleH_ -= ROT_SPEED;

			// �|�P�W�O�x�ȉ��ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x�𑫂�
			if (angleH_ < -DX_PI_F)
			{
				angleH_ += DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琅���p�x���v���X����
		if (input.IsPressed("Right"))
		{
			angleH_ += ROT_SPEED;

			// �P�W�O�x�ȏ�ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x������
			if (angleH_ > DX_PI_F)
			{
				angleH_ -= DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���}�C�i�X����
		if (input.IsPressed("Up"))
		{
			angleV_ -= ROT_SPEED;

			// ������p�x�ȉ��ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (angleV_ < -DX_PI_F / 2.0f + 0.6f)
			{
				angleV_ = -DX_PI_F / 2.0f + 0.6f;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���v���X����
		if (input.IsPressed("Down"))
		{
			angleV_ += ROT_SPEED;

			// ������p�x�ȏ�ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (angleV_ > DX_PI_F / 2.0f - 0.6f)
			{
				angleV_ = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// �J�����̒����_�̓v���C���[���W����K��l���������W
	targetPos_ = VAdd(targetActor_->GetPos(), VGet(0.0f, TARGET_HEIGHT_OFFSET, 0.0f));

	// �J�����̍��W�����肷��
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;

		// ���������̉�]�͂x����]
		RotY = MGetRotY(angleH_);

		// ���������̉�]�͂y����] )
		RotZ = MGetRotZ(angleV_);

		// �J��������v���C���[�܂ł̏����������Z�b�g
		Camera_Player_Length = CAMERA_ARM_LENGTH;

		// �J�����̍��W���Z�o
		// �w���ɃJ�����ƃv���C���[�Ƃ̋����������L�т��x�N�g����
		// ����������]( �y����] )���������Ɛ���������]( �x����] )���čX��
		// �����_�̍��W�𑫂������̂��J�����̍��W
		pos_ = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), targetPos_);

		// �����_����J�����̍��W�܂ł̊ԂɃX�e�[�W�̃|���S�������邩���ׂ�
		HRes = stage_.CollCheckCapsule(targetPos_, pos_, COLLISION_SIZE);
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0)
		{
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;

			// ������Ȃ��ʒu�܂Ńv���C���[�ɋ߂Â�

			// �|���S���ɓ�����Ȃ��������Z�b�g
			NotHitLength = 0.0f;

			// �|���S���ɓ����鋗�����Z�b�g
			HitLength = Camera_Player_Length;
			do
			{
				// �����邩�ǂ����e�X�g���鋗�����Z�b�g( ������Ȃ������Ɠ����鋗���̒��� )
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;

				// �e�X�g�p�̃J�������W���Z�o
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), targetPos_);

				// �V�������W�ŕǂɓ����邩�e�X�g
				HRes = stage_.CollCheckCapsule(targetPos_, TestPosition, COLLISION_SIZE);
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0)
				{
					// ���������瓖���鋗���� TestLength �ɕύX����
					HitLength = TestLength;
				}
				else
				{
					// ������Ȃ������瓖����Ȃ������� TestLength �ɕύX����
					NotHitLength = TestLength;
				}

				// HitLength �� NoHitLength ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
			} while (HitLength - NotHitLength > 0.1f);

			// �J�����̍��W���Z�b�g
			pos_ = TestPosition;
		}
	}

	// �J�����̏������C�u�����̃J�����ɔ��f������
	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));
	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

const VECTOR& Camera::GetTargetPos() const
{
	return targetPos_;
}

void Camera::SetTargetActor(std::shared_ptr<Actor> target)
{
	targetActor_ = target;
}
