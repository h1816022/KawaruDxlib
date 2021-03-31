#include <DxLib.h>
#include <vector>
#include "Character.h"
#include "Stage.h"
#include "../AnimationComponent.h"
#include "../Systems/File.h"
#include "../Systems/FileManager.h"

namespace
{
	// 影の大きさ
	constexpr float SHADOW_SIZE = 200.0f;

	// 影の落ちる高さ
	constexpr float SHADOW_HEIGHT = 700.0f;

	// 処理するコリジョンポリゴンの最大数
	constexpr int MAX_HIT_COLLISION = 2048;

	// 周囲のポリゴン検出に使用する球のサイズ
	constexpr float COLLISION_SPHERE_SIZE = 2048.0f;

	// 壁押し出し処理の最大試行回数
	constexpr int HIT_TRY_NUM = 16;

	// 押し出し処理でスライドさせる距離
	constexpr float HIT_SLIDE_LENGTH = 5.0f;

	// ジャンプ力
	constexpr float JUMP_POWER = 100.0f;

	// 足を踏み外したときのジャンプ力
	constexpr float FALL_UP_POWER = 20.0f;

	// 重力
	constexpr float GRAVITY = 6.0f;

	// 回転速度
	constexpr float ROT_SPEED = 0.2f;
}

// 衝突判定をするポリゴンに関するデータ
struct HitCheckPolyData
{
	HitCheckPolyData()
	{
		wall.resize(MAX_HIT_COLLISION);
		floor.resize(MAX_HIT_COLLISION);
	}

	unsigned int wallNum = 0;
	unsigned int floorNum = 0;

	std::vector<MV1_COLL_RESULT_POLY*> wall;	// 壁ポリゴン
	std::vector<MV1_COLL_RESULT_POLY*> floor;	// 床ポリゴン
};

Character::Character(Scene& scene, const Stage& stage, const float hitWidth, const float hitHeight, const float posX, const float posY, const float posZ):
	Actor(scene, posX, posY, posZ), stage_(stage),
	updater_(&Character::IdleUpdate),
	shadowHandle_(LoadGraph(L"Resources/Images/Shadow.tga")), hitWidth_(hitWidth), hitHeight_(hitHeight),
	moveDirection_(VGet(1.0f, 0.0f, 0.0f)), moveVec_(VGet(0.0f, 0.0f, 0.0f)),
	animationComponent_(std::make_unique<AnimationComponent>(modelHandle_))
{
	jumpSE_ = FileManager::Instance().Load(L"Resources/Sounds/Jump.mp3")->GetHandle();
}

Character::Character(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const Stage& stage, const float hitWidth, const float hitHeight, const float posX, const float posY, const float posZ):
	Actor(scene, modelFilePath, motionFilePath, posX, posY, posZ), stage_(stage),
	updater_(&Character::IdleUpdate),
	shadowHandle_(LoadGraph(L"Resources/Images/Shadow.tga")), hitWidth_(hitWidth), hitHeight_(hitHeight), 
	moveDirection_(VGet(1.0f, 0.0f, 0.0f)), moveVec_(VGet(0.0f, 0.0f, 0.0f)),
	animationComponent_(std::make_unique<AnimationComponent>(modelHandle_))
{
	jumpSE_ = FileManager::Instance().Load(L"Resources/Sounds/Jump.mp3")->GetHandle();
}

Character::~Character()
{
}

void Character::Update(const Input& input)
{
	MATRIX localMatrix;

	MV1ResetFrameUserLocalMatrix(modelHandle_, 2);

	localMatrix = MV1GetFrameLocalMatrix(modelHandle_, 2);

	localMatrix.m[3][2] = 0.0f;

	MV1SetFrameUserLocalMatrix(modelHandle_, 2, localMatrix);

	UpdateMoveStopTime();

	animationComponent_->Update();
}

void Character::ChangeUpadater(UPDATE_TYPE type)
{
	if (nowUpdateType_ == type)
	{
		return;
	}

	nowUpdateType_ = type;

	switch (type)
	{
	case UPDATE_TYPE::Idle:
		updater_ = &Character::IdleUpdate;
		break;

	case UPDATE_TYPE::Run:
		updater_ = &Character::RunUpdate;
		break;

	case UPDATE_TYPE::Jump:
		updater_ = &Character::JumpUpdate;
		break;

	case UPDATE_TYPE::Destroy:
		updater_ = &Character::DestroyUpdate;
		break;

	default:
		break;
	}
}

Capsule3D Character::GetCollisionCapsule(const VECTOR& pos)
{
	return Capsule3D(pos, VAdd(pos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_);
}

bool Character::Destroy()
{
	ChangeUpadater(UPDATE_TYPE::Destroy);

	return true;
}

void Character::UpdatePos(const VECTOR& moveVector)
{
	bool moveFlag = false;
	bool hitFlag = false;

	MV1_COLL_RESULT_POLY_DIM hitDim;
	VECTOR nowPos;
	HitCheckPolyData polyData;

	nowPos = VAdd(pos_, moveVector);

	hitDim = stage_.CheckHitSphere(pos_, COLLISION_SPHERE_SIZE + VSize(moveVector));

	constexpr float MOVE_THRESHOLD = 0.01f;
	moveFlag = (fabs(moveVector.x) > MOVE_THRESHOLD || fabs(moveVector.z) > MOVE_THRESHOLD);

	GetWallPolyAndFloorPoly(polyData, hitDim);

	if (collisionEnabled_)
	{
		if (polyData.wallNum != 0)
		{
			hitFlag = CheckHitWithWall(moveFlag, polyData, moveVector, nowPos);

			// 押し出し
			if (hitFlag)
			{
				Extrude(polyData, nowPos);
			}
		}

		if (polyData.floorNum != 0)
		{
			CheckHitWithFloor(moveFlag, polyData, nowPos);
		}
	}

	SetPos(nowPos);

	// 検出したプレイヤーの周囲のポリゴン情報を開放
	MV1CollResultPolyDimTerminate(hitDim);
}

void Character::GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData)
{
	for (int i = 0; i < HitData.HitNum; ++i)
	{
		if (IsVerticalToXZPlane(HitData.Dim[i]))
		{
			if (IsAbovePos(HitData.Dim[i]))
			{
				if (outPolyData.wallNum < MAX_HIT_COLLISION)
				{
					outPolyData.wall[outPolyData.wallNum] = &HitData.Dim[i];
					++outPolyData.wallNum;
				}
			}
		}
		else
		{
			if (outPolyData.floorNum < MAX_HIT_COLLISION)
			{
				outPolyData.floor[outPolyData.floorNum] = &HitData.Dim[i];
				++outPolyData.floorNum;
			}
		}
	}
}

bool Character::CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos)
{
	bool isHit = false;
	MV1_COLL_RESULT_POLY* poly;

	if (moveFlag)
	{
		for (int i = 0; i < polyData.wallNum; ++i)
		{
			poly = polyData.wall[i];

			auto myCollision = GetCollisionCapsule(nowPos);

			if (!CheckHitCapsuleAndPoly(myCollision, *poly))
			{
				continue;
			}

			isHit = true;

			VECTOR slideVec;

			slideVec = VCross(moveVector, poly->Normal);
			slideVec = VCross(poly->Normal, slideVec);

			nowPos = VAdd(pos_, slideVec);

			int j;

			for (j = 0; j < polyData.wallNum; ++j)
			{
				poly = polyData.wall[j];

				myCollision = GetCollisionCapsule(nowPos);

				if (CheckHitCapsuleAndPoly(myCollision, *poly))
				{
					break;
				}
			}

			if (j == polyData.wallNum)
			{
				isHit = false;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < polyData.wallNum; ++i)
		{
			poly = polyData.wall[i];

			auto coll = GetCollisionCapsule(nowPos);

			if (CheckHitCapsuleAndPoly(coll, *poly))
			{
				isHit = true;
				break;
			}
		}
	}

	return isHit;
}

void Character::CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	HITRESULT_LINE result;

	if (updater_ == &Character::JumpUpdate && jumpPower_ > 0.0f)
	{
		CheckHitCeiling(polyData, nowPos);
	}
	else
	{
		float maxY = 0.0f;

		bool isHit = false;

		for (int i = 0; i < polyData.floorNum; ++i)
		{
			poly = polyData.floor[i];

			if (updater_ == &Character::JumpUpdate)
			{
				constexpr float LITTLE_AHEAD_OFFSET = -1.0f;
				result = CheckHitLineAndPoly(VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), VAdd(nowPos, VGet(0.0f, LITTLE_AHEAD_OFFSET, 0.0f)), *poly);
			}
			else
			{
				constexpr float BODY_OFFSET = -40.0f;
				result = CheckHitLineAndPoly(VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), VAdd(nowPos, VGet(0.0f, BODY_OFFSET, 0.0f)), *poly);
			}

			if (!result.HitFlag)
			{
				continue;
			}

			if (isHit && maxY > result.Position.y)
			{
				continue;
			}

			isHit = true;
			maxY = result.Position.y;
		}

		ReactionToHitFloor(moveFlag, isHit, nowPos, maxY);
	}
}

bool Character::IsVerticalToXZPlane(const tagMV1_COLL_RESULT_POLY& poly) const
{
	constexpr float THRESHOLD = 0.000001f;
	return (poly.Normal.y < THRESHOLD) && (poly.Normal.y > -THRESHOLD);
}

bool Character::IsAbovePos(const tagMV1_COLL_RESULT_POLY& poly) const
{
	return poly.Position[0].y > pos_.y + 1.0f ||
		poly.Position[1].y > pos_.y + 1.0f ||
		poly.Position[2].y > pos_.y + 1.0f;
}

void Character::Jump()
{
	ChangeUpadater(UPDATE_TYPE::Jump);

	jumpPower_ = JUMP_POWER;

	moveVec_.y = jumpPower_;

	animationComponent_->ChangeAnim(ANIM_NAME::Jump);

	PlaySoundMem(jumpSE_, DX_PLAYTYPE_BACK);
}


void Character::Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	for (int tryNum = 0; tryNum < HIT_TRY_NUM; ++tryNum)
	{
		int polyIndex;

		for (polyIndex = 0; polyIndex < polyData.wallNum; ++polyIndex)
		{
			poly = polyData.wall[polyIndex];

			auto coll = GetCollisionCapsule(nowPos);

			if (!CheckHitCapsuleAndPoly(coll, *poly))
			{
				continue;
			}

			// 押し出す
			nowPos = VAdd(nowPos, VScale(VNorm(VSub(nowPos, poly->HitPosition)), HIT_SLIDE_LENGTH));

			int wallIndex;

			coll = GetCollisionCapsule(nowPos);

			for (wallIndex = 0; wallIndex < polyData.wallNum; ++wallIndex)
			{
				poly = polyData.wall[wallIndex];
				if (CheckHitCapsuleAndPoly(coll, *poly))
				{
					break;
				}
			}

			// 押し出し終わり
			if (wallIndex == polyData.wallNum)
			{
				break;
			}
		}

		// 押し出し終わり
		if (polyIndex != polyData.wallNum)
		{
			break;
		}
	}
}

void Character::DrawShadow()
{
	MV1_COLL_RESULT_POLY_DIM hitResDim;
	MV1_COLL_RESULT_POLY* hitRes;
	VERTEX3D vertex[3];
	VECTOR slideVec;

	SetUseLighting(false);

	SetUseZBuffer3D(true);

	// テクスチャアドレスモードを CLAMP にする(テクスチャの端より先は端のドットが延々続く)
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	hitResDim = stage_.CheckHitCapsule(pos_, VAdd(pos_, VGet(0.0f, -SHADOW_HEIGHT, 0.0f)), SHADOW_SIZE);

	// 頂点データで変化が無い部分
	vertex[0].dif = GetColorU8(255, 255, 255, 255);
	vertex[0].spc = GetColorU8(0, 0, 0, 0);
	vertex[0].su = 0.0f;
	vertex[0].sv = 0.0f;
	vertex[1] = vertex[0];
	vertex[2] = vertex[0];

	hitRes = hitResDim.Dim;
	for (int i = 0; i < hitResDim.HitNum; ++i, ++hitRes)
	{
		vertex[0].pos = hitRes->Position[0];
		vertex[1].pos = hitRes->Position[1];
		vertex[2].pos = hitRes->Position[2];

		// ちょっと持ち上げて重ならないようにする
		slideVec = VScale(hitRes->Normal, 0.5f);
		vertex[0].pos = VAdd(vertex[0].pos, slideVec);
		vertex[1].pos = VAdd(vertex[1].pos, slideVec);
		vertex[2].pos = VAdd(vertex[2].pos, slideVec);

		vertex[0].dif.a = 0;
		vertex[1].dif.a = 0;
		vertex[2].dif.a = 0;
		if (hitRes->Position[0].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[0].dif.a = 128 * (1.0f - fabs(hitRes->Position[0].y - pos_.y) / SHADOW_HEIGHT);
		}

		if (hitRes->Position[1].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[1].dif.a = 128 * (1.0f - fabs(hitRes->Position[1].y - pos_.y) / SHADOW_HEIGHT);
		}

		if (hitRes->Position[2].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[2].dif.a = 128 * (1.0f - fabs(hitRes->Position[2].y - pos_.y) / SHADOW_HEIGHT);
		}

		// ＵＶ値
		vertex[0].u = (hitRes->Position[0].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[0].v = (hitRes->Position[0].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].u = (hitRes->Position[1].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].v = (hitRes->Position[1].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].u = (hitRes->Position[2].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].v = (hitRes->Position[2].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;

		DrawPolygon3D(vertex, 1, shadowHandle_, true);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hitResDim);

	SetUseLighting(true);
	SetUseZBuffer3D(false);
}

void Character::StopMove(float time)
{
	canMove_ = false;

	moveStopTime_ = time;
	nowMoveStopTime_ = 0;
}

void Character::IdleUpdate(const Input& input)
{
}

void Character::RunUpdate(const Input& input)
{
}

void Character::JumpUpdate(const Input& input)
{
	jumpPower_ -= GRAVITY;

	if (jumpPower_ < 0.0f && animationComponent_->CheckNowAnim(ANIM_NAME::Jump))
	{
		animationComponent_->ChangeAnim(ANIM_NAME::Fall);
	}

	moveVec_.y = jumpPower_;
}

void Character::DestroyUpdate(const Input& input)
{
	isDead_ = true;
}

void Character::UpdateAngle()
{
	float targetAngle;
	float diff;

	targetAngle = atan2(moveDirection_.x, moveDirection_.z);

	diff = CalcAngleDiff(targetAngle);

	if (diff > 0.0f)
	{
		diff -= ROT_SPEED;
		if (diff < 0.0f)
		{
			diff = 0.0f;
		}
	}
	else
	{
		diff += ROT_SPEED;
		if (diff > 0.0f)
		{
			diff = 0.0f;
		}
	}

	angle_ = targetAngle - diff;
	MV1SetRotationXYZ(modelHandle_, VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

const UPDATE_TYPE& Character::GetNowUpdateType() const
{
	return nowUpdateType_;
}

float Character::CalcAngleDiff(float target)const
{
	float ret = target - angle_;

	if (ret < -DX_PI_F)
	{
		ret += DX_TWO_PI_F;
	}
	else if (ret > DX_PI_F)
	{
		ret -= DX_TWO_PI_F;
	}

	return ret;
}

void Character::CheckHitCeiling(const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	HITRESULT_LINE result;

	float minY = 0.0f;

	bool isHit = false;

	for (int i = 0; i < polyData.floorNum; ++i)
	{
		poly = polyData.floor[i];

		result = CheckHitLineAndPoly(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), *poly);

		if (!result.HitFlag)
		{
			continue;
		}

		if (isHit && minY < result.Position.y)
		{
			continue;
		}

		isHit = true;
		minY = result.Position.y;
	}

	if (isHit)
	{
		nowPos.y = minY - hitHeight_;
		jumpPower_ = -jumpPower_;
	}
}

void Character::ReactionToHitFloor(bool moveFlag, bool isHit, VECTOR& nowPos, float maxY)
{
	if (isHit)
	{
		nowPos.y = max(nowPos.y, maxY);

		jumpPower_ = 0.0f;

		if (updater_ == &Character::JumpUpdate)
		{
			if (moveFlag)
			{
				animationComponent_->ChangeAnim(ANIM_NAME::Walk);
				ChangeUpadater(UPDATE_TYPE::Run);
			}
			else
			{
				animationComponent_->ChangeAnim(ANIM_NAME::Idle);
				ChangeUpadater(UPDATE_TYPE::Idle);
			}

			animationComponent_->ForceBlend();
		}
	}
	else
	{
		if (updater_ != &Character::JumpUpdate)
		{
			ChangeUpadater(UPDATE_TYPE::Jump);

			jumpPower_ = FALL_UP_POWER;

			animationComponent_->ChangeAnim(ANIM_NAME::Fall);
		}
	}
}

void Character::UpdateMoveStopTime()
{
	if (moveStopTime_ == -1)
	{
		return;
	}
	
	++nowMoveStopTime_;

	if (nowMoveStopTime_ >= moveStopTime_)
	{
		moveStopTime_ = -1;
		nowMoveStopTime_ = 0;

		canMove_ = true;
	}
}
