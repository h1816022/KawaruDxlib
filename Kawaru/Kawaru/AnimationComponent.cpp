#include <DxLib.h>
#include "AnimationComponent.h"

namespace
{
	// アニメーションの再生速度
	constexpr float ANIM_PLAY_SPEED = 1.0f;

	// アニメーションのブレンド速度
	constexpr float ANIM_BLEND_SPEED = 0.1f;

	// アニメーションアタッチ番号の、アタッチなしのときの値
	constexpr int ANIM_ATTACH_NUM_INVALID = -1;
}

AnimationComponent::AnimationComponent(int modelHandle):
	modelHandle_(modelHandle)
{
	InitAnimData();
}

AnimationComponent::~AnimationComponent()
{
}

void AnimationComponent::ForceBlend()
{
	animBlendRate_ = 1.0f;
}

int AnimationComponent::GetNowAnimTotalTime()
{
	return GetAnimData(animAttachNum1_).totalTime;
}

bool AnimationComponent::EndAnim()
{
	auto data = GetAnimData(animAttachNum1_);
	return !data.isLoop && animPlayCount1_ == data.totalTime;
}

void AnimationComponent::InitAnimData()
{
	animData_[ANIM_NAME::Idle].isLoop = true;
	animData_[ANIM_NAME::Walk].isLoop = true;
	animData_[ANIM_NAME::Jump].isLoop = false;
	animData_[ANIM_NAME::Fall].isLoop = true;
	animData_[ANIM_NAME::Dead].isLoop = false;
	animData_[ANIM_NAME::Call].isLoop = false;

	for (int i = 0; i < static_cast<int>(ANIM_NAME::Max); ++i)
	{
		animData_[static_cast<ANIM_NAME>(i)].totalTime = MV1GetAnimTotalTime(modelHandle_, i);
	}
}

void AnimationComponent::UpdateAnimBlendRate()
{
	if (animBlendRate_ >= 1.0f)
	{
		return;
	}

	animBlendRate_ = min(animBlendRate_ + ANIM_BLEND_SPEED, 1.0f);
}

bool AnimationComponent::CheckNowAnim(ANIM_NAME target) const
{
	return MV1GetAttachAnim(modelHandle_, animAttachNum1_) == static_cast<int>(target);
}

int AnimationComponent::AttachAnim(ANIM_NAME animName)
{
	return MV1AttachAnim(modelHandle_, static_cast<int>(animName));
}

void AnimationComponent::DetachAnim(int animAttachNum)
{
	MV1DetachAnim(modelHandle_, animAttachNum);
}

float AnimationComponent::GetAnimTotalTime(int animAttachNum)
{
	return MV1GetAttachAnimTotalTime(modelHandle_, animAttachNum);
}

void AnimationComponent::SetAnimTime(int animAttachNum, int count)
{
	MV1SetAttachAnimTime(modelHandle_, animAttachNum, count);
}

void AnimationComponent::SetAnimBlendRate(int animAttachNum, float rate)
{
	MV1SetAttachAnimBlendRate(modelHandle_, animAttachNum, rate);
}

AnimData AnimationComponent::GetAnimData(int animAttachNum)
{
	return animData_[static_cast<ANIM_NAME>(MV1GetAttachAnim(modelHandle_, animAttachNum))];
}

void AnimationComponent::UpdateAnimCount(int animAttachNum, float& animCount)
{
	auto data = GetAnimData(animAttachNum);

	const float TOTAL_TIME = data.totalTime;

	animCount += ANIM_PLAY_SPEED;

	if (animCount >= TOTAL_TIME)
	{
		animCount = (data.isLoop) ? fmod(animCount, TOTAL_TIME) : TOTAL_TIME;
	}
}

void AnimationComponent::ChangeAnim(ANIM_NAME playAnim)
{
	// 再生中のモーション２が有効だったらデタッチする
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		DetachAnim(animAttachNum2_);
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	animAttachNum2_ = animAttachNum1_;
	animPlayCount2_ = animPlayCount1_;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	animAttachNum1_ = AttachAnim(playAnim);
	animPlayCount1_ = 0.0f;

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	animBlendRate_ = (animAttachNum2_ == ANIM_ATTACH_NUM_INVALID) ? 1.0f : 0.0f;
}

void AnimationComponent::Update()
{
	UpdateAnimBlendRate();

	// 再生しているアニメーション１の処理
	if (animAttachNum1_ != ANIM_ATTACH_NUM_INVALID)
	{
		UpdateAnimCount(animAttachNum1_, animPlayCount1_);

		// 変更した再生時間をモデルに反映させる
		SetAnimTime(animAttachNum1_, animPlayCount1_);

		// アニメーション１のモデルに対する反映率をセット
		SetAnimBlendRate(animAttachNum1_, animBlendRate_);
	}

	// 再生しているアニメーション２の処理
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		UpdateAnimCount(animAttachNum2_, animPlayCount2_);

		// 変更した再生時間をモデルに反映させる
		SetAnimTime(animAttachNum2_, animPlayCount2_);

		// アニメーション２のモデルに対する反映率をセット
		SetAnimBlendRate(animAttachNum2_, 1.0f - animBlendRate_);
	}
}