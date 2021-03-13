#include "Ghost.h"
#include "../Systems/Input.h"
#include "Stage.h"
#include "../NavMesh/NavMeshMoveComponent.h"
#include "../NavMesh/NavMesh.h"
#include "Camera.h"

namespace
{
	// �ړ����x
	constexpr float DEFAULT_MOVE_SPEED = 35.0f;

	// �����蔻��J�v�Z���̔��a
	constexpr float HIT_WIDTH = 15.0f;

	// �����蔻��J�v�Z���̍���
	constexpr float HIT_HEIGHT = 15.0f;

	// �ڕW�n�_�ɒB�����Ƃ݂Ȃ����a
	constexpr float GOAL_REACH_RADIUS = 125.0f;
}

Ghost::Ghost(Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Character(stage, HIT_WIDTH * 4, HIT_HEIGHT * 2, posX, posY, posZ), camera_(camera)
{
	floatingOffset_ = VGet(0.0f, 120.0f, 0.0f);
	navMeshMoveComponent_ = std::make_unique<NavMeshMoveComponent>(*this, stage_, NAV_TYPE::floated);
}

Ghost::~Ghost()
{
}

void Ghost::Update(const Input& input)
{
	if (navMeshMoveComponent_->CheckPathExists())
	{
		auto paths = navMeshMoveComponent_->GetPaths();

		moveDirection_ = VNorm(VSub(VAdd(navMeshMoveComponent_->GetNextTargetPos(), floatingOffset_), pos_));
		moveVec_ = VScale(moveDirection_, DEFAULT_MOVE_SPEED);
	}
	else
	{
		if (input.IsTriggered("DebugMode"))
		{
			if (camera_.CanSeePlayer(p))
			{
				navMeshMoveComponent_->CalcPath(pos_, p);
			}
		}
		moveVec_ = VGet(0.0f, 0.0f, 0.0f);
	}

	UpdatePos(moveVec_);
	UpdateAngle();
	UpdateAnim();

	navMeshMoveComponent_->Update(GOAL_REACH_RADIUS);

	camera_.SetPos(pos_);
}

void Ghost::Draw()
{
	DrawShadow();

	DrawSphere3D(VAdd(pos_, VGet(0.0f, HIT_WIDTH * 2, 0.0f)), HIT_WIDTH * 2, 10, 0xff0000, 0xff0000, false);
	DrawSphere3D(p, 50, 10, 0xff0000, 0xff0000, true);

	navMeshMoveComponent_->Draw();
}
