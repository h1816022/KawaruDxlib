#include "Camera.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../Geometry.h"

namespace
{
	// 回転速度
	constexpr float ROT_SPEED = 0.05f;

	// 注視点に設定したアクターからの高さのオフセット、一人称視点時
	constexpr float TARGET_HEIGHT_OFFSET_FIRST = 800.0f;

	// 注視点に設定したアクターからの高さのオフセット、三人称視点時
	constexpr float TARGET_HEIGHT_OFFSET_THIRD = 400.0f;

	// 視点座標から注視点までの距離、一人称視点時
	constexpr float CAMERA_ARM_LENGTH_FIRST = 1.0f;

	// 視点座標から注視点までの距離、三人称視点時
	constexpr float CAMERA_ARM_LENGTH_THIRD = 1.0f;

	// 遮蔽物検出用コリジョンの大きさ
	constexpr float COLLISION_SIZE = 50.0f;
}

Camera::Camera(const Stage& stage, const float posX, const float posY, const float posZ) :
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
	UpdatePos();

	UpdateAngle(input);

	if (input.IsPressed("Camera") || CanSeePlayer())
	{
		targetPos_ = VAdd(targetActor_->GetPos(), VGet(0.0f, targetHeightOffset_, 0.0f));
		c = true;
	}
	else
	{
		const MATRIX rotY = MGetRotY(angleH_);
		const MATRIX rotZ = MGetRotZ(angleV_);

		if (c)
		{
			c = false;
			VECTOR tmpTargetPos = VGet(1.0f, 0.0f, 0.0f);

			VECTOR toPlayerVec = VNorm(VSub(targetPos_, pos_));
			VECTOR toTmpTargetVec = VNorm(VSub(tmpTargetPos, pos_));

			VECTOR toPlayerXZ = VGet(toPlayerVec.x, 0.0f, toPlayerVec.z);
			VECTOR toTmpTargetXZ = VGet(toTmpTargetVec.x, 0.0f, toTmpTargetVec.z);

			a = acosf(VDot(toPlayerXZ, toTmpTargetXZ) / (GetLength(toPlayerXZ) * GetLength(toTmpTargetXZ)));
			angleH_ = (a > 0.0f) ? -a : a;

			VECTOR toPlayerXY = VGet(toPlayerVec.x, toPlayerVec.y, 0.0f);
			VECTOR toTmpTargetXY = VGet(toTmpTargetVec.x, toTmpTargetVec.y, 0.0f);

			b = acosf(VDot(toPlayerXY, toTmpTargetXY) / (GetLength(toPlayerXY) * GetLength(toTmpTargetXY)));
			b = (a > 1.6f) ? b - 3.2f : b;
			angleV_ = b;
		}

		targetPos_ = VAdd(VTransform(VTransform(VGet(1.0f, 0.0f, 0.0f), rotZ), rotY), pos_);
	}

	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));

	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

void Camera::Draw()
{
	DrawFormatString(0, 0, 0xffffff, L"%f", a);
	DrawFormatString(0, 25, 0xffffff, L"%f", angleH_);
}

void Camera::SetPos(const VECTOR& pos)
{
	pos_ = pos;
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
	const MATRIX rotY = MGetRotY(angleH_);
	const MATRIX rotZ = MGetRotZ(angleV_);

	// カメラの座標を算出
	//pos_ = VAdd(VTransform(VTransform(VGet(-armLength_, 0.0f, 0.0f), rotZ), rotY), targetPos_);

	//pos_ = targetActor_->GetPos();

	//UpdateArmLength(rotY, rotZ);
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

void Camera::UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ)
{
	MV1_COLL_RESULT_POLY_DIM hitResult;
	int hitNum;

	hitResult = stage_.CheckHitCapsule(targetPos_, pos_, COLLISION_SIZE);
	hitNum = hitResult.HitNum;
	MV1CollResultPolyDimTerminate(hitResult);
	if (hitNum != 0)
	{
		float notHitLength;
		float hitLength;
		float testLength;
		VECTOR testPosition;

		notHitLength = 0.0f;

		hitLength = armLength_;
		do
		{
			testLength = notHitLength + (hitLength - notHitLength) / 2.0f;

			testPosition = VAdd(VTransform(VTransform(VGet(-testLength, 0.0f, 0.0f), rotZ), rotY), targetPos_);

			hitResult = stage_.CheckHitCapsule(targetPos_, testPosition, COLLISION_SIZE);
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

bool Camera::CanSeePlayer()
{
	VECTOR minPos = player_->GetLineTraceSamplingOffsets()[0];
	VECTOR maxPos = player_->GetLineTraceSamplingOffsets()[0];

	for (auto& targetPosOffset : player_->GetLineTraceSamplingOffsets())
	{
		VECTOR targetPos = VAdd(player_->GetPos(), targetPosOffset);

		auto hitResult = stage_.CheckHitLine(pos_, targetPos);
		if (!hitResult.HitFlag)
		{
			return true;
		}
	}

	return false;
}

bool Camera::CanSeePlayer(VECTOR& visiblePos)
{
	VECTOR minPos = player_->GetLineTraceSamplingOffsets()[0];
	VECTOR maxPos = player_->GetLineTraceSamplingOffsets()[0];

	for (auto& targetPosOffset : player_->GetLineTraceSamplingOffsets())
	{
		VECTOR targetPos = VAdd(player_->GetPos(), targetPosOffset);

		auto hitResult = stage_.CheckHitLine(pos_, targetPos);
		if (!hitResult.HitFlag)
		{
			visiblePos = targetPos;
			return true;
		}
	}

	return false;
}