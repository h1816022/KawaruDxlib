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
	armLength_(CAMERA_ARM_LENGTH_THIRD),
	updaterByMode_(&Camera::UpdatePlayerFollowMode),
	nowShakeOffset_(VECTOR()),
	targetShakeOffset_(VECTOR())
{
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(false);

	angle_.horizontal = DX_PI_F;
	angle_.vertical = 0.0f;
}

Camera::~Camera()
{
}

void Camera::Update(const Input& input)
{
	UpdatePos();

	//UpdateAngle(input);

	(this->*updaterByMode_)();

	setEye_ = Lerp(setEye_, pos_, 0.2f);

	setTarget_ = Lerp(setTarget_, targetPos_, 0.2f);

	SetCameraPositionAndTarget_UpVecY(setEye_, VAdd(setTarget_, VScale(nowShakeOffset_, min(GetLength(VSub(targetPos_, pos_)) / 6000.0f, 1.0f))));

	nowShakeOffset_ = Lerp(nowShakeOffset_, targetShakeOffset_, 0.001f);

	if (++shakeCount_ > 120)
	{
		shakeCount_ = 0;

		targetShakeOffset_ = VScale(RandomVector(), (static_cast<float>(GetRand(500)) + 1.0f) * 10.0f);
	}
}

void Camera::Draw()
{
	DrawFormatString(0, 0, 0xffffff, L"%f", GetLength(VSub(targetActor_->GetPos(), pos_)));
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
	auto analogInpoutData = input.GetAnalogInput(ANALOG_INPUT_TYPE::Right);

	if (analogInpoutData.horizontal != 0.0f)
	{
		angle_.horizontal += ROT_SPEED * analogInpoutData.horizontal;
	}

	if (analogInpoutData.vertical != 0.0f)
	{
		angle_.vertical += ROT_SPEED * analogInpoutData.vertical;
	}

	ClampAngle();
}

void Camera::UpdatePos()
{
	const MATRIX rotY = MGetRotY(angle_.horizontal);
	const MATRIX rotZ = MGetRotZ(angle_.vertical);

	// カメラの座標を算出
	//pos_ = VAdd(VTransform(VTransform(VGet(-armLength_, 0.0f, 0.0f), rotZ), rotY), targetPos_);

	//pos_ = targetActor_->GetPos();

	//UpdateArmLength(rotY, rotZ);
}

void Camera::ClampAngle()
{
	if (angle_.horizontal < -DX_PI_F)
	{
		angle_.horizontal += DX_TWO_PI_F;
	}
	else if (angle_.horizontal > DX_PI_F)
	{
		angle_.horizontal -= DX_TWO_PI_F;
	}

	if (angle_.vertical < -DX_PI_F / 2.0f + 0.6f)
	{
		angle_.vertical = -DX_PI_F / 2.0f + 0.6f;
	}
	else if (angle_.vertical > DX_PI_F / 2.0f - 0.6f)
	{
		angle_.vertical = DX_PI_F / 2.0f - 0.6f;
	}
}

Angle Camera::CalcAngle(const VECTOR& nowVec, const VECTOR& targetVec)
{
	Angle ret;

	VECTOR toPlayerXZ = VGet(targetVec.x, 0.0f, targetVec.z);
	VECTOR toFrontXZ = VGet(nowVec.x, 0.0f, nowVec.z);

	float newAngleH = 0.0f;
	float newAngleV = 0.0f;

	float size = (GetLength(toPlayerXZ) * GetLength(toFrontXZ));
	if (size != 0.0f)
	{
		newAngleH = acosf(VDot(toPlayerXZ, toFrontXZ) / size);
		const MATRIX rotY = MGetRotY(angle_.horizontal);
		const MATRIX rotZ = MGetRotZ(angle_.vertical);

		VECTOR v = VGet(0.0f, 0.0f, 1.0f);
		VECTOR v2 = VGet(1.0f, 0.0f, 0.0f);
		VECTOR p = VTransform(VTransform(VGet(1.0f, 0.0f, 0.0f), rotZ), rotY);

		auto s = VDot(v, VSub(p, v2));
		newAngleH = (s <= 0.0f) ? -newAngleH : newAngleH;
	}

	VECTOR toPlayerXY = VGet(targetVec.x, targetVec.y, 0.0f);
	VECTOR toFrontXY = VGet(nowVec.x, nowVec.y, 0.0f);

	size = (GetLength(toPlayerXY) * GetLength(toFrontXY));
	if (size != 0.0f)
	{
		newAngleV = acosf(VDot(toPlayerXY, toFrontXY) / size);
		//newAngleV = (newAngleV > (DX_PI_F / 2)) ? DX_PI_F - newAngleV : newAngleV;
	}

	ret.horizontal = newAngleH;
	ret.vertical = angle_.vertical;

	return ret;
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

void Camera::LostPlayer()
{
	followingPlayer_ = false;

	Angle newAngle = CalcAngle(VNorm(VSub(VGet(1.0f, 0.0f, 0.0f), pos_)), VNorm(VSub(targetPos_, pos_)));
	angle_.horizontal = newAngle.horizontal;
	angle_.vertical = newAngle.vertical;
}

void Camera::UpdatePlayerFollowMode()
{
	if (CanSeePlayer())
	{
		targetPos_ = VAdd(targetActor_->GetPos(), VGet(0.0f, targetHeightOffset_, 0.0f));
		followingPlayer_ = true;
	}
	else
	{
		const MATRIX rotY = MGetRotY(angle_.horizontal);
		const MATRIX rotZ = MGetRotZ(angle_.vertical);

		if (followingPlayer_)
		{
			followingPlayer_ = false;
			VECTOR tmpTargetPos = VGet(1.0f, 0.0f, 0.0f);

			VECTOR toPlayerVec = VNorm(VSub(targetPos_, pos_));
			VECTOR toTmpTargetVec = VNorm(VSub(tmpTargetPos, pos_));

			VECTOR toPlayerXZ = VGet(toPlayerVec.x, 0.0f, toPlayerVec.z);
			VECTOR toTmpTargetXZ = VGet(toTmpTargetVec.x, 0.0f, toTmpTargetVec.z);

			auto newAngleH = acosf(VDot(toPlayerXZ, toTmpTargetXZ) / (GetLength(toPlayerXZ) * GetLength(toTmpTargetXZ)));
			angle_.horizontal = (newAngleH > 0.0f) ? -newAngleH : newAngleH;

			VECTOR toPlayerXY = VGet(toPlayerVec.x, toPlayerVec.y, 0.0f);
			VECTOR toTmpTargetXY = VGet(toTmpTargetVec.x, toTmpTargetVec.y, 0.0f);

			auto newAngleV = acosf(VDot(toPlayerXY, toTmpTargetXY) / (GetLength(toPlayerXY) * GetLength(toTmpTargetXY)));
			newAngleV = (newAngleH > DX_PI_F / 2) ? newAngleV - DX_PI_F : newAngleV;
			angle_.vertical = newAngleV;
		}

		//targetPos_ = VAdd(VTransform(VTransform(VGet(1.0f, 0.0f, 0.0f), rotZ), rotY), pos_);
	}

	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));

	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

void Camera::UpdateTargetFollowMode()
{
}

void Camera::UpdateManualMode()
{
	if (followingPlayer_)
	{
		LostPlayer();
	}

	const MATRIX rotY = MGetRotY(angle_.horizontal);
	const MATRIX rotZ = MGetRotZ(angle_.vertical);

	targetPos_ = VAdd(VTransform(VTransform(VGet(1.0f, 0.0f, 0.0f), rotZ), rotY), pos_);
}

void Camera::ChangeMode(CAMERA_MODE mode)
{
	if (mode_ == mode)
	{
		return;
	}

	mode_ = mode;

	switch (mode)
	{
	case CAMERA_MODE::PlayerFollow:
		updaterByMode_ = &Camera::UpdatePlayerFollowMode;
		break;

	case CAMERA_MODE::TargetFollow:
		updaterByMode_ = &Camera::UpdateTargetFollowMode;
		break;

	case CAMERA_MODE::Manual:
		updaterByMode_ = &Camera::UpdateManualMode;
		break;

	default:
		break;
	}
}

bool Camera::GetFollowingPlayerFlag()
{
	return followingPlayer_;
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

bool Camera::GetVisiblePlayerPos(VECTOR& visiblePos)
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