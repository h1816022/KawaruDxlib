#include "Camera.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../Tools/Geometry.h"

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

Camera::Camera(Scene& scene, const Stage& stage, const float posX, const float posY, const float posZ) :
	Actor(scene, posX, posY, posZ),
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

	(this->*updaterByMode_)();

	setEye_ = Lerp(setEye_, pos_, 0.2f);

	setTarget_ = Lerp(setTarget_, targetPos_, 0.2f);

	UpdateCameraShake();
}

void Camera::UpdateCameraShake()
{
	SetCameraPositionAndTarget_UpVecY(setEye_, VAdd(setTarget_, VScale(nowShakeOffset_, min(GetLength(VSub(targetPos_, pos_)) / 6000.0f, 1.0f))));

	nowShakeOffset_ = Lerp(nowShakeOffset_, targetShakeOffset_, 0.001f);

	if (++shakeCount_ > 120)
	{
		shakeCount_ = 0;

		targetShakeOffset_ = VScale(RandomVector(), (static_cast<float>(GetRand(500)) + 1.0f) * 10.0f);
	}
}

const VECTOR& Camera::GetTargetPos() const
{
	return targetPos_;
}

void Camera::SetPlayer(std::shared_ptr<Player> player)
{
	player_ = player;
	targetPos_ = player_->GetPos();
}

void Camera::UpdatePos()
{
	const MATRIX rotY = MGetRotY(angle_.horizontal);
	const MATRIX rotZ = MGetRotZ(angle_.vertical);
}

void Camera::UpdatePlayerFollowMode()
{
	if (CanSeePlayer())
	{
		targetPos_ = Lerp(targetPos_, VAdd(player_->GetPos(), VGet(0.0f, targetHeightOffset_, 0.0f)), 0.1f);
		followingPlayer_ = true;
	}
	else
	{
		followingPlayer_ = false;
	}

	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));

	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

void Camera::UpdateGameClear()
{
	targetPos_ = Lerp(targetPos_, VAdd(player_->GetPos(), VGet(0.0f, targetHeightOffset_, 0.0f)), 0.1f);

	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));

	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
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

	case CAMERA_MODE::GameEnd:
		updaterByMode_ = &Camera::UpdateGameClear;
		break;

	default:
		break;
	}
}

bool Camera::GetFollowingPlayerFlag()
{
	return followingPlayer_;
}

void Camera::EndGame()
{
	ChangeMode(CAMERA_MODE::GameEnd);
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