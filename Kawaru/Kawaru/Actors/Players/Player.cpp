#include <Dxlib.h>
#include <array>
#include <memory>
#include "Player.h"
#include "../Camera.h"
#include "../Ghost.h"
#include "../Stage.h"
#include "../../NavMesh/NavMesh.h"
#include "../../NavMesh/NavMeshPath.h"
#include "../../Systems/Input.h"
#include "../../AnimationComponent.h"
#include "../../Systems/File.h"
#include "../../Systems/FileManager.h"
#include "../../Scenes/Scene.h"
#include "../../Application.h"

namespace
{
	// 移動速度
	constexpr float MAX_MOVE_SPEED = 40.0f;
		
	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 150.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 680.0f;

	// 体の幅
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;

	// 開始時、おばけを呼べる回数
	constexpr int INIT_CALL_NUM = 10;

	constexpr float SCALE = 35.0f;
}

Player::Player(Scene& scene, Camera& camera, Ghost& ghost, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(scene, L"Resources/Models/sotai/素体A.pmx", L"Resources/Models/Sotai/Animations/Sotai", stage, HIT_WIDTH, HIT_HEIGHT, posX, posY, posZ),
	camera_(camera),
	ghost_(ghost)
{
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));

	MV1SetScale(modelHandle_, VGet(SCALE, SCALE, SCALE));

	int materialNum = MV1GetMaterialNum(modelHandle_);

	auto& fileManager = FileManager::Instance();

	fingerSnapSE_ = fileManager.Load(L"Resources/Sounds/FingerSnap.mp3")->GetHandle();

	tag_ = L"Player";

	callNum_ = INIT_CALL_NUM;

	animationComponent_->ChangeAnim(ANIM_NAME::Idle);
}

Player::~Player()
{
}

void Player::Update(const Input& input)
{
	if (Application::Instance().CheckIsGameClear())
	{
		return;
	}

	Character::Update(input);

	if (canMove_)
	{
		(this->*updater_)(input);

		UpdateAngle();

		UpdatePos(moveVec_);
	}
	else
	{
		moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	}
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

bool Player::Destroy()
{
	if (GetNowUpdateType() != UPDATE_TYPE::Idle &&
		GetNowUpdateType() != UPDATE_TYPE::Run)
	{
		return false;
	}

	ChangeUpadater(UPDATE_TYPE::Destroy);

	EndGame();

	return true;
}

void Player::EndGame()
{
	camera_.EndGame();
	ghost_.EndGame();
}

void Player::AddCallNum()
{
	++callNum_;
}

int Player::GetCallNum() const
{
	return callNum_;
}

void Player::IdleUpdate(const Input& input)
{
	UpdateMove(input);

	if (input.IsTriggered("Jump"))
	{
		Jump();
		return;
	}

	if (input.IsTriggered("Call"))
	{
		if (callNum_ <= 0)
		{
			return;
		}

		if (CallGhost())
		{
			Application::Instance().AddCallCount();
			--callNum_;
			return;
		}
	}

	if (moveFlag_)
	{
		animationComponent_->ChangeAnim(ANIM_NAME::Walk);
		ChangeUpadater(UPDATE_TYPE::Run);
	}
}

void Player::RunUpdate(const Input& input)
{
	UpdateMove(input);

	if (input.IsTriggered("Jump"))
	{
		Jump();
		return;
	}

	if (input.IsTriggered("Call"))
	{
		if (callNum_ <= 0)
		{
			return;
		}

		if (CallGhost())
		{
			Application::Instance().AddCallCount();
			--callNum_;
			return;
		}
	}

	if (!moveFlag_)
	{
		animationComponent_->ChangeAnim(ANIM_NAME::Idle);
		ChangeUpadater(UPDATE_TYPE::Idle);
	}
}

void Player::JumpUpdate(const Input& input)
{
	UpdateMove(input);

	Character::JumpUpdate(input);
}

void Player::DestroyUpdate(const Input& input)
{
	auto& app = Application::Instance();

	if (app.CheckIsGameOver())
	{
		return;
	}

	if (GetNowUpdateType() == UPDATE_TYPE::Jump)
	{
		return;
	}

	if (!animationComponent_->CheckNowAnim(ANIM_NAME::Dead))
	{
		animationComponent_->ChangeAnim(ANIM_NAME::Dead);

		moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	}

	if (animationComponent_->EndAnim())
	{
		app.SetIsGameOver();
	}
}

void Player::CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec)
{
	// 前後入力
	upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
	upMoveVec.y = 0.0f;

	// 左右入力
	leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

	upMoveVec = VNorm(upMoveVec);
	leftMoveVec = VNorm(leftMoveVec);
}

bool Player::CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input)
{
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	bool moveFlag = false;

	auto analogInpoutData = input.GetAnalogInput(ANALOG_INPUT_TYPE::Left);

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

void Player::UpdateMove(const Input& input)
{
	VECTOR upMoveVec;
	VECTOR leftMoveVec;

	CalcUnitMoveVector(upMoveVec, leftMoveVec);

	moveFlag_ = CalcMoveVector(moveVec_, upMoveVec, leftMoveVec, input);

	if (moveFlag_)
	{
		moveDirection_ = VNorm(moveVec_);

		const float MOVE_SPEED_RATE = input.GetAnalogInput(ANALOG_INPUT_TYPE::Left).Length();

		moveVec_ = VScale(moveDirection_, MAX_MOVE_SPEED * MOVE_SPEED_RATE);
	}
}

bool Player::CallGhost()
{
	if (!ghost_.Call())
	{
		return false;
	}

	animationComponent_->ChangeAnim(ANIM_NAME::Call);

	// 呼ぶ時のアニメーションに掛かる総時間
	const int CALL_ANIM_TOTAL_TIME = animationComponent_->GetNowAnimTotalTime();
	
	// 呼ぶ音の再生待ち時間	
	const int CALL_SE_TIME = CALL_ANIM_TOTAL_TIME - 10;

	StopMove(CALL_ANIM_TOTAL_TIME);

	ChangeUpadater(UPDATE_TYPE::Idle);

	FileManager::Instance().DelayPlaySound(fingerSnapSE_, CALL_SE_TIME);

	return true;
}