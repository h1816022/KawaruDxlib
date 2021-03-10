#include <Dxlib.h>
#include <array>
#include <memory>
#include "Player.h"
#include "../Camera.h"
#include "../Stage.h"
#include "../NavMesh.h"
#include "../../NavMeshPath.h"
#include "../../Systems/Input.h"

namespace
{
	// �ړ����x
	constexpr float DEFAULT_MOVE_SPEED = 30.0f;
	
	// �W�����v��
	constexpr float JUMP_POWER = 100.0f;
	
	// ���𓥂݊O�����Ƃ��̃W�����v��
	constexpr float FALL_UP_POWER = 20.0f;
	
	// �d��
	constexpr float GRAVITY = 6.0f;

	// ��]���x
	constexpr float ROT_SPEED = 0.2f;

	// ��������R���W�����|���S���̍ő吔
	constexpr int MAX_HIT_COLLISION = 2048;

	// �ǉ����o�������̍ő厎�s��
	constexpr int HIT_TRY_NUM = 16;

	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
	constexpr float COLLISION_SPHERE_SIZE = 2048.0f;
	
	// �����蔻��J�v�Z���̔��a
	constexpr float HIT_WIDTH = 150.0f;

	// �����蔻��J�v�Z���̍���
	constexpr float HIT_HEIGHT = 680.0f;

	// �����o�������ŃX���C�h�����鋗��
	constexpr float HIT_SLIDE_LENGTH = 5.0f;

	// �A�j���[�V�����̍Đ����x
	constexpr float ANIM_PLAY_SPEED = 400.0f;
	
	// �A�j���[�V�����̃u�����h���x
	constexpr float ANIM_BLEND_SPEED = 0.1f;

	// �A�j���[�V�����A�^�b�`�ԍ��́A�A�^�b�`�Ȃ��̂Ƃ��̒l
	constexpr int ANIM_ATTACH_NUM_INVALID = -1;

	// �e�̑傫��
	constexpr float SHADOW_SIZE = 200.0f;
	
	// �e�̗����鍂��
	constexpr float SHADOW_HEIGHT = 700.0f;

	// �̂̕�
	constexpr float BODY_WIDTH = HIT_WIDTH / 3 * 2;
}

// �Փ˔��������|���S���Ɋւ���f�[�^
struct HitCheckPolyData
{
	unsigned int wallNum = 0;
	unsigned int floorNum = 0;

	std::array<MV1_COLL_RESULT_POLY*, MAX_HIT_COLLISION> wall;	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	std::array<MV1_COLL_RESULT_POLY*, MAX_HIT_COLLISION> floor;	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
};

Player::Player(const Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(L"Models/DxChara.x", posX, posY, posZ), camera_(camera), stage_(stage), 
	updater_(&Player::IdleUpdate),
	moveDirection_(VGet(1.0f, 0.0f, 0.0f)),
	shadowHandle_(LoadGraph(L"Images/Shadow.tga"))
{
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, 1.0f, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, -BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(-BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));
	lineTraceSamplingOffsets_.emplace_back(VGet(BODY_WIDTH, HIT_HEIGHT, BODY_WIDTH));
}

Player::~Player()
{
}

void Player::Update(const Input& input)
{
	VECTOR upMoveVec;
	VECTOR leftMoveVec;
	VECTOR moveVec;
	bool moveFlag;

	MATRIX localMatrix;

	MV1ResetFrameUserLocalMatrix(modelHandle_, 2);

	localMatrix = MV1GetFrameLocalMatrix(modelHandle_, 2);

	localMatrix.m[3][2] = 0.0f;

	MV1SetFrameUserLocalMatrix(modelHandle_, 2, localMatrix);

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	CalcUnitMoveVector(upMoveVec, leftMoveVec);

	// �e�����̃x�N�g�����A�ړ��x�N�g�����Z�o
	moveFlag = CalcMoveVector(moveVec, upMoveVec, leftMoveVec, input);

	if (moveFlag)
	{
		moveDirection_ = VNorm(moveVec);

		const float MOVE_SPEED_RATE = input.GetAnalogInput(AnalogInputType::LEFT).Length();

		moveVec = VScale(moveDirection_, DEFAULT_MOVE_SPEED * MOVE_SPEED_RATE);

		if (updater_ == &Player::IdleUpdate)
		{
			ChangeAnim(PLAYER_ANIM_NAME::WALK);
			updater_ = &Player::RunUpdate;
		}
	}
	else
	{
		if (updater_ == &Player::RunUpdate)
		{
			ChangeAnim(PLAYER_ANIM_NAME::IDLE);
			updater_ = &Player::IdleUpdate;
		}
	}

	(this->*updater_)(input);

	if (updater_ == &Player::JumpUpdate)
	{
		jumpPower_ -= GRAVITY;

		if (jumpPower_ < 0.0f && CheckNowAnim(animAttachNum1_, PLAYER_ANIM_NAME::JUMP))
		{
			ChangeAnim(PLAYER_ANIM_NAME::FALL);
		}

		moveVec.y = jumpPower_;
	}

	UpdateAngle();

	Move(moveVec);

	UpdateAnim();
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);

	DrawShadow();

	NavMesh& navMesh = stage_.GetNavMesh();

	auto result = navMesh.CheckHitLine(VAdd(pos_, VGet(0.0f, 100.0f, 0.0f)), VSub(pos_, VGet(0.0f, 100.0f, 0.0f)));

	auto goal = navMesh.CheckHitLine(VAdd(pos_, VGet(3000.0f, 100.0f, 4000.0f)), VSub(pos_, VGet(-3000.0f, 100.0f, -4000.0f)));

	//DrawLine3D(result.Position[0], result.Position[1], 0xff0000);
	//DrawLine3D(result.Position[1], result.Position[2], 0xff0000);
	//DrawLine3D(result.Position[2], result.Position[0], 0xff0000);

	//DrawLine3D(goal.Position[0], goal.Position[1], 0x00ff00);
	//DrawLine3D(goal.Position[1], goal.Position[2], 0x00ff00);
	//DrawLine3D(goal.Position[2], goal.Position[0], 0x00ff00);

	NavMeshPath path;
	navMesh.FindPath(path, result.PolygonIndex, result.HitPosition, goal.PolygonIndex, goal.HitPosition);

	for (int i = 0; i < path.GetWaypoints().size() - 1; ++i)
	{
		DrawLine3D(path.GetWaypoints()[i].pos, path.GetWaypoints()[i + 1].pos, 0xffffff);
	}

	auto p = path.GetStraightPath(0.0f);
	for (int i = 0; i < (p.size() - 1); ++i)
	{
		DrawLine3D(VAdd(p[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(p[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0xff0000);
	}

	p = path.GetStraightPath(0.5f);
	for (int i = 0; i < (p.size() - 1); ++i)
	{
		DrawLine3D(VAdd(p[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(p[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0x00ff00);
	}

	p = path.GetStraightPath(1.0f);
	for (int i = 0; i < (p.size() - 1); ++i)
	{
		DrawLine3D(VAdd(p[i], VGet(0.0f, 0.0f, 0.0f)), VAdd(p[i + 1], VGet(0.0f, 0.0f, 0.0f)), 0x0000ff);
	}
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
}

void Player::RunUpdate(const Input& input)
{
	if (input.IsTriggered("Jump"))
	{
		Jump();
		return;
	}
}

void Player::JumpUpdate(const Input& input)
{
}

void Player::Jump()
{
	updater_ = &Player::JumpUpdate;

	jumpPower_ = JUMP_POWER;

	ChangeAnim(PLAYER_ANIM_NAME::JUMP);
}

void Player::Move(const VECTOR& moveVector)
{
	// �ړ�������
	bool moveFlag;

	// �Փ˂����m������
	bool hitFlag;

	// �v���C���[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	MV1_COLL_RESULT_POLY_DIM hitDim;

	// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^
	MV1_COLL_RESULT_POLY* poly;

	VECTOR nowPos;

	// �Փ˔��������|���S���Ɋւ���f�[�^
	HitCheckPolyData polyData;

	nowPos = VAdd(pos_, moveVector);

	// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
	hitDim = stage_.CheckHitSphere(pos_, COLLISION_SPHERE_SIZE + VSize(moveVector));

	// x����y��������0.01f�ȏ�ړ������ꍇ�͈ړ������Ƃ���
	moveFlag = (fabs(moveVector.x) > 0.01f || fabs(moveVector.z) > 0.01f);

	GetWallPolyAndFloorPoly(polyData, hitDim);

	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (polyData.wallNum != 0)
	{
		hitFlag = CheckHitWithWall(moveFlag, polyData, moveVector, nowPos);

		// �������Ă����牟���o��
		if (hitFlag)
		{
			Extrude(polyData, nowPos);
		}
	}

	// ���|���S���Ƃ̓����蔻��
	if (polyData.floorNum != 0)
	{
		CheckHitWithFloor(moveFlag, polyData, nowPos);
	}

	// ���W�̏�������
	pos_ = nowPos;

	// ���f�����W�̍X�V
	MV1SetPosition(modelHandle_, pos_);

	// ���o�����v���C���[�̎��͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(hitDim);
}

void Player::CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec)
{
	// �O�����
	upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
	upMoveVec.y = 0.0f;

	// ���E����
	leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

	// ��̃x�N�g���𐳋K��
	upMoveVec = VNorm(upMoveVec);
	leftMoveVec = VNorm(leftMoveVec);
}

bool Player::CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input)
{
	// ���̃t���[���ł̈ړ��x�N�g����������
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	bool moveFlag = false;

	auto analogInpoutData = input.GetAnalogInput(AnalogInputType::LEFT);

	// ���E�ړ�
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

void Player::UpdateAngle()
{
	float targetAngle;			// �ڕW�p�x
	float diffAngle;			// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	targetAngle = atan2(moveDirection_.x, moveDirection_.z);

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	diffAngle = CalcAngleDiff(targetAngle);

	// �p�x�̍����O�ɋ߂Â���
	if (diffAngle > 0.0f)
	{
		// �����v���X�̏ꍇ�͈���
		diffAngle -= ROT_SPEED;
		if (diffAngle < 0.0f)
		{
			diffAngle = 0.0f;
		}
	}
	else
	{
		// �����}�C�i�X�̏ꍇ�͑���
		diffAngle += ROT_SPEED;
		if (diffAngle > 0.0f)
		{
			diffAngle = 0.0f;
		}
	}

	// ���f���̊p�x���X�V
	angle_ = targetAngle - diffAngle;
	MV1SetRotationXYZ(modelHandle_, VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

float Player::CalcAngleDiff(float target)const
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

void Player::ChangeAnim(PLAYER_ANIM_NAME playAnim)
{
	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		DetachAnim(animAttachNum2_);
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	animAttachNum2_ = animAttachNum1_;
	animPlayCount2_ = animPlayCount1_;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	animAttachNum1_ = AttachAnim(playAnim);
	animPlayCount1_ = 0.0f;

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	animBlendRate_ = (animAttachNum2_ == ANIM_ATTACH_NUM_INVALID) ? 1.0f : 0.0f;
}

void Player::UpdateAnim()
{
	float totalTime;

	UpdateAnimBlendRate();

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (animAttachNum1_ != ANIM_ATTACH_NUM_INVALID)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		totalTime = GetAnimTotalTime(animAttachNum1_);

		// �Đ����Ԃ�i�߂�
		animPlayCount1_ += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (animPlayCount1_ >= totalTime)
		{
			animPlayCount1_ = fmod(animPlayCount1_, totalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		SetAnimTime(animAttachNum1_, animPlayCount1_);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		SetAnimBlendRate(animAttachNum1_, animBlendRate_);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		totalTime = GetAnimTotalTime(animAttachNum2_);

		// �Đ����Ԃ�i�߂�
		animPlayCount2_ += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (animPlayCount2_ > totalTime)
		{
			animPlayCount2_ = fmod(animPlayCount2_, totalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		SetAnimTime(animAttachNum2_, animPlayCount2_);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		SetAnimBlendRate(animAttachNum2_, 1.0f - animBlendRate_);
	}
}

void Player::UpdateAnimBlendRate()
{
	if (animBlendRate_ >= 1.0f)
	{
		return;
	}

	animBlendRate_ = min(animBlendRate_ + ANIM_BLEND_SPEED, 1.0f);
}

bool Player::CheckNowAnim(int animAttachNum, PLAYER_ANIM_NAME target) const
{
	return MV1GetAttachAnim(modelHandle_, animAttachNum) == static_cast<int>(target);
}

int Player::AttachAnim(PLAYER_ANIM_NAME animName)
{
	return MV1AttachAnim(modelHandle_, static_cast<int>(animName));
}

void Player::DetachAnim(int animAttachNum)
{
	MV1DetachAnim(modelHandle_, animAttachNum);
}

float Player::GetAnimTotalTime(int animattachNum)
{
	return MV1GetAttachAnimTotalTime(modelHandle_, animattachNum);
}

void Player::SetAnimTime(int animAttachNum, int count)
{
	MV1SetAttachAnimTime(modelHandle_, animAttachNum, count);
}

void Player::SetAnimBlendRate(int animAttachNum, float rate)
{
	MV1SetAttachAnimBlendRate(modelHandle_, animAttachNum, rate);
}

void Player::DrawShadow()
{
	MV1_COLL_RESULT_POLY_DIM hitResDim;
	MV1_COLL_RESULT_POLY* hitRes;
	VERTEX3D vertex[3];
	VECTOR slideVec;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(false);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(true);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	hitResDim = stage_.CheckHitCapsule(pos_, VAdd(pos_, VGet(0.0f, -SHADOW_HEIGHT, 0.0f)), SHADOW_SIZE);

	// ���_�f�[�^�ŕω��������������Z�b�g
	vertex[0].dif = GetColorU8(255, 255, 255, 255);
	vertex[0].spc = GetColorU8(0, 0, 0, 0);
	vertex[0].su = 0.0f;
	vertex[0].sv = 0.0f;
	vertex[1] = vertex[0];
	vertex[2] = vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	hitRes = hitResDim.Dim;
	for (int i = 0; i < hitResDim.HitNum; i++, hitRes++)
	{
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		vertex[0].pos = hitRes->Position[0];
		vertex[1].pos = hitRes->Position[1];
		vertex[2].pos = hitRes->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		slideVec = VScale(hitRes->Normal, 0.5f);
		vertex[0].pos = VAdd(vertex[0].pos, slideVec);
		vertex[1].pos = VAdd(vertex[1].pos, slideVec);
		vertex[2].pos = VAdd(vertex[2].pos, slideVec);

		// �|���S���̕s�����x��ݒ肷��
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

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		vertex[0].u = (hitRes->Position[0].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[0].v = (hitRes->Position[0].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].u = (hitRes->Position[1].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].v = (hitRes->Position[1].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].u = (hitRes->Position[2].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].v = (hitRes->Position[2].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(vertex, 1, shadowHandle_, true);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(hitResDim);

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(true);

	// �y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(false);
}

void Player::GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData)
{
	// ���o���ꂽ�|���S���̐������J��Ԃ�
	for (int i = 0; i < HitData.HitNum; i++)
	{
		// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
		if (HitData.Dim[i].Normal.y < 0.000001f && HitData.Dim[i].Normal.y > -0.000001f)
		{
			// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
			if (HitData.Dim[i].Position[0].y > pos_.y + 1.0f ||
				HitData.Dim[i].Position[1].y > pos_.y + 1.0f ||
				HitData.Dim[i].Position[2].y > pos_.y + 1.0f)
			{
				// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (outPolyData.wallNum < MAX_HIT_COLLISION)
				{
					// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					outPolyData.wall[outPolyData.wallNum] = &HitData.Dim[i];

					// �ǃ|���S���̐������Z����
					outPolyData.wallNum++;
				}
			}
		}
		else
		{
			// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
			if (outPolyData.floorNum < MAX_HIT_COLLISION)
			{
				// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
				outPolyData.floor[outPolyData.floorNum] = &HitData.Dim[i];

				// ���|���S���̐������Z����
				outPolyData.floorNum++;
			}
		}
	}
}

bool Player::CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos)
{
	// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
	bool isHit = false;

	MV1_COLL_RESULT_POLY* poly;

	// �ړ��������ǂ����ŏ����𕪊�
	if (moveFlag == true)
	{
		// �ǃ|���S���̐������J��Ԃ�
		for (int i = 0; i < polyData.wallNum; i++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			poly = polyData.wall[i];

			// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == false)
			{
				continue;
			}

			// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
			isHit = true;

			// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
			{
				VECTOR slideVec;	// �v���C���[���X���C�h������x�N�g��

				// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
				slideVec = VCross(moveVector, poly->Normal);

				// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
				// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
				slideVec = VCross(poly->Normal, slideVec);

				// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
				nowPos = VAdd(pos_, slideVec);
			}

			int j;

			// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
			for (j = 0; j < polyData.wallNum; j++)
			{
				// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				poly = polyData.wall[j];

				// �������Ă����烋�[�v���甲����
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == true) break;
			}

			// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
			// �ǂɓ��������t���O��|������Ń��[�v���甲����
			if (j == polyData.wallNum)
			{
				isHit = false;
				break;
			}
		}
	}
	else
	{
		// �ړ����Ă��Ȃ��ꍇ�̏���

		// �ǃ|���S���̐������J��Ԃ�
		for (int i = 0; i < polyData.wallNum; i++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			poly = polyData.wall[i];

			// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == true)
			{
				isHit = true;
				break;
			}
		}
	}

	return isHit;
}

void Player::CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	HITRESULT_LINE lineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����

	bool isHit = false;

	// �W�����v�����㏸���̏ꍇ�͏����𕪊�
	if (updater_ == &Player::JumpUpdate && jumpPower_ > 0.0f)
	{
		float minY;

		// �V��ɓ����Ԃ��鏈�����s��

		// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
		minY = 0.0f;

		// �����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���

		// ���|���S���̐������J��Ԃ�
		for (int i = 0; i < polyData.floorNum; i++)
		{
			// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
			poly = polyData.floor[i];

			// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
			lineRes = HitCheck_Line_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);

			// �ڐG���Ă��Ȃ������牽�����Ȃ�
			if (!lineRes.HitFlag)
			{
				continue;
			}

			// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
			if (isHit && minY < lineRes.Position.y)
			{
				continue;
			}

			// �|���S���ɓ��������t���O�𗧂Ă�
			isHit = true;

			// �ڐG�����x���W��ۑ�����
			minY = lineRes.Position.y;
		}

		// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
		if (isHit)
		{
			// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
			nowPos.y = minY - HIT_HEIGHT;

			// �x�������̑��x�͔��]
			jumpPower_ = -jumpPower_;
		}
	}
	else
	{
		float maxY;

		// ���~�����W�����v���ł͂Ȃ��ꍇ�̏���

		// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
		isHit = false;

		// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
		maxY = 0.0f;

		// ���|���S���̐������J��Ԃ�
		for (int i = 0; i < polyData.floorNum; i++)
		{
			// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
			poly = polyData.floor[i];

			// �W�����v�����ǂ����ŏ����𕪊�
			if (updater_ == &Player::JumpUpdate)
			{
				// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
				lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), VAdd(nowPos, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
			}
			else
			{
				// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
				lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), VAdd(nowPos, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
			}

			// �������Ă��Ȃ������牽�����Ȃ�
			if (!lineRes.HitFlag)
			{
				continue;
			}

			// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
			if (isHit && maxY > lineRes.Position.y)
			{
				continue;
			}

			// �|���S���ɓ��������t���O�𗧂Ă�
			isHit = true;

			// �ڐG�����x���W��ۑ�����
			maxY = lineRes.Position.y;
		}

		// ���|���S���ɓ����������ǂ����ŏ����𕪊�
		if (isHit)
		{
			// ���������ꍇ

			// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
			nowPos.y = maxY;

			// �x�������̈ړ����x�͂O��
			jumpPower_ = 0.0f;

			// �����W�����v���������ꍇ�͒��n��Ԃɂ���
			if (updater_ == &Player::JumpUpdate)
			{
				// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
				if (moveFlag)
				{
					// �ړ����Ă���ꍇ�͑����Ԃ�
					ChangeAnim(PLAYER_ANIM_NAME::WALK);
					updater_ = &Player::RunUpdate;
				}
				else
				{
					// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
					ChangeAnim(PLAYER_ANIM_NAME::IDLE);
					updater_ = &Player::IdleUpdate;
				}

				// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
				animBlendRate_ = 1.0f;
			}
		}
		else
		{
			// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
			if (updater_ != &Player::JumpUpdate)
			{
				// �W�����v���ɂ���
				updater_ = &Player::JumpUpdate;

				// ������Ƃ����W�����v����
				jumpPower_ = FALL_UP_POWER;

				// �A�j���[�V�����͗������̂��̂ɂ���
				ChangeAnim(PLAYER_ANIM_NAME::FALL);
			}
		}
	}
}

void Player::Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
	for (int tryNum = 0; tryNum < HIT_TRY_NUM; tryNum++)
	{
		// �ǃ|���S���̐������J��Ԃ�
		int polyIndex;

		for (polyIndex = 0; polyIndex < polyData.wallNum; polyIndex++)
		{
			// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
			poly = polyData.wall[polyIndex];

			// �v���C���[�Ɠ������Ă��邩�𔻒�
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == false)
			{
				continue;
			}

			// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
			nowPos = VAdd(nowPos, VScale(poly->Normal, HIT_SLIDE_LENGTH));

			// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
			int wallIndex;

			for (wallIndex = 0; wallIndex < polyData.wallNum; wallIndex++)
			{
				// �������Ă����烋�[�v�𔲂���
				poly = polyData.wall[wallIndex];
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == true)
				{
					break;
				}
			}

			// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
			if (wallIndex == polyData.wallNum)
			{
				break;
			}
		}

		// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
		if (polyIndex != polyData.wallNum)
		{
			break;
		}
	}
}
