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
	// 移動速度
	constexpr float DEFAULT_MOVE_SPEED = 30.0f;
		
	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 150.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 680.0f;

	// 体の幅
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;
}

Player::Player(const Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(/*L"Models/Hatune/初音ミク.pmd"*/L"Models/DxChara.x", L"Models/Hatune/Hatune", stage, HIT_WIDTH, HIT_HEIGHT, posX, posY, posZ), camera_(camera)
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

	// プレイヤーの移動方向のベクトルを算出
	CalcUnitMoveVector(upMoveVec, leftMoveVec);

	// 各成分のベクトルより、移動ベクトルを算出
	moveFlag_ = CalcMoveVector(moveVec_, upMoveVec, leftMoveVec, input);

	if (moveFlag_)
	{
		moveDirection_ = VNorm(moveVec_);

		const float MOVE_SPEED_RATE = input.GetAnalogInput(AnalogInputType::LEFT).Length();

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

	//NavMesh& navMesh = stage_.GetNavMesh();

	//auto result = navMesh.CheckHitLine(VAdd(pos_, VGet(0.0f, 5000.0f, 0.0f)), VSub(pos_, VGet(0.0f, 5000.0f, 0.0f)));

	//auto goal = navMesh.CheckHitLine(VAdd(pos_, VGet(-3000.0f, 5000.0f, 4000.0f)), VSub(pos_, VGet(3000.0f, 5000.0f, -4000.0f)));

	//DrawLine3D(result.Position[0], result.Position[1], 0xffff00);
	//DrawLine3D(result.Position[1], result.Position[2], 0xffff00);
	//DrawLine3D(result.Position[2], result.Position[0], 0xffff00);

	//DrawLine3D(goal.Position[0], goal.Position[1], 0x00ffff);
	//DrawLine3D(goal.Position[1], goal.Position[2], 0x00ffff);
	//DrawLine3D(goal.Position[2], goal.Position[0], 0x00ffff);

	//NavMeshPath path;
	//if (!navMesh.FindPath(path, result.PolygonIndex, result.HitPosition, goal.PolygonIndex, goal.HitPosition))
	//{
	//	return;
	//}

	//for (int i = 0; i < path.GetWaypoints().size() - 1; ++i)
	//{
	//	auto n = navMesh.GetMeshRef();

	//	int id = path.GetWaypoints()[i].id;
	//	if (id == -1)
	//	{
	//		continue;
	//	}

	//	DrawLine3D(n.Vertexs[n.Polygons[id].VIndex[0]].Position, 
	//		n.Vertexs[n.Polygons[id].VIndex[1]].Position
	//		, 0xffffff);
	//	DrawLine3D(n.Vertexs[n.Polygons[id].VIndex[1]].Position, 
	//		n.Vertexs[n.Polygons[id].VIndex[2]].Position
	//		, 0xffffff);
	//	DrawLine3D(n.Vertexs[n.Polygons[id].VIndex[2]].Position, 
	//		n.Vertexs[n.Polygons[id].VIndex[0]].Position
	//		, 0xffffff);
	//}

	//auto p = path.GetStraightPath(0.5f);
	//for (int i = 0; i < (p.size() - 1); ++i)
	//{
	//	DrawLine3D(VAdd(p[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(p[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0x00ff00);
	//}
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
	// 前後入力
	upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
	upMoveVec.y = 0.0f;

	// 左右入力
	leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

	// 二つのベクトルを正規化
	upMoveVec = VNorm(upMoveVec);
	leftMoveVec = VNorm(leftMoveVec);
}

bool Player::CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input)
{
	// このフレームでの移動ベクトルを初期化
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	bool moveFlag = false;

	auto analogInpoutData = input.GetAnalogInput(AnalogInputType::LEFT);

	// 左右移動
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