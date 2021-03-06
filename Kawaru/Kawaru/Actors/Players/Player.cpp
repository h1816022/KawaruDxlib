#include <Dxlib.h>
#include "Player.h"
#include "../Camera.h"
#include "../Stage.h"
#include "../../Systems/Input.h"

namespace
{
	// �ړ����x
	constexpr float MOVE_SPEED = 30.0f;
	
	// �W�����v��
	constexpr float JUMP_POWER = 100.0f;
	
	// ���𓥂݊O�����Ƃ��̃W�����v��
	constexpr float FALL_UP_POWER = 20.0f;
	
	// �d��
	constexpr float GRAVITY = 3.0f;

	// ��]���x
	constexpr float ROT_SPEED = 0.2f;

	// ��������R���W�����|���S���̍ő吔
	constexpr int MAX_HIT_COLLISION = 2048;

	// �ǉ����o�������̍ő厎�s��
	constexpr int HIT_TRY_NUM = 16;

	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
	constexpr float COLLISION_SPHERE_SIZE = 2048.0f;
	
	// �����蔻��J�v�Z���̔��a
	constexpr float HIT_WIDTH = 200.0f;

	// �����蔻��J�v�Z���̍���
	constexpr float HIT_HEIGHT = 700.0f;

	// �����o�������ŃX���C�h�����鋗��
	constexpr float HIT_SLIDE_LENGTH = 5.0f;

	// �A�j���[�V�����̍Đ����x
	constexpr float ANIM_PLAY_SPEED = 250.0f;
	
	// �A�j���[�V�����̃u�����h���x
	constexpr float ANIM_BLEND_SPEED = 0.1f;

	// �e�̑傫��
	constexpr float SHADOW_SIZE = 200.0f;
	
	// �e�̗����鍂��
	constexpr float SHADOW_HEIGHT = 700.0f;
}

Player::Player(const Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(L"Models/DxChara.x", posX, posY, posZ), camera_(camera), stage_(stage), updater_(&Player::IdleUpdate)
{
	moveDirection_ = VGet(1.0f, 0.0f, 0.0f);
	shadowHandle_ = LoadGraph(L"Images/Shadow.tga");
}

Player::~Player()
{
}

void Player::Update(const Input& input)
{
	(this->*updater_)();

	VECTOR upMoveVec;
	VECTOR leftMoveVec;
	VECTOR moveVec;
	int moveFlag;

	MATRIX localMatrix;

	// ���[�U�[�s�����������
	MV1ResetFrameUserLocalMatrix(modelHandle_, 2);

	// ���݂̃��[�g�t���[���̍s����擾����
	localMatrix = MV1GetFrameLocalMatrix(modelHandle_, 2);

	// �y�������̕��s�ړ������𖳌��ɂ���
	localMatrix.m[3][2] = 0.0f;

	// ���[�U�[�s��Ƃ��ĕ��s�ړ������𖳌��ɂ����s������[�g�t���[���ɃZ�b�g����
	MV1SetFrameUserLocalMatrix(modelHandle_, 2, localMatrix);

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	{
		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���̓J�����̎�����������x�����𔲂�������
		upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
		upMoveVec.y = 0.0f;

		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���͏���������Ƃ��̕����x�N�g���Ƃx���̃v���X�����̃x�N�g���ɐ����ȕ���
		leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

		// ��̃x�N�g���𐳋K��( �x�N�g���̒������P�D�O�ɂ��邱�� )
		upMoveVec = VNorm(upMoveVec);
		leftMoveVec = VNorm(leftMoveVec);
	}

	// ���̃t���[���ł̈ړ��x�N�g����������
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	// �ړ��������ǂ����̃t���O��������Ԃł́u�ړ����Ă��Ȃ��v��\���O�ɂ���
	moveFlag = 0;

	// �ړ�����
	// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�č������Ɉړ�����
	if (input.IsPressed("Left"))
	{
		// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
		moveVec = VAdd(moveVec, leftMoveVec);

		// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		moveFlag = 1;
	}
	else
		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�ĉE�����Ɉړ�����
		if (input.IsPressed("Right"))
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
			moveVec = VAdd(moveVec, VScale(leftMoveVec, -1.0f));

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			moveFlag = 1;
		}

	// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă�������Ɉړ�����
	if (input.IsPressed("Up"))
	{
		// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
		moveVec = VAdd(moveVec, upMoveVec);

		// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		moveFlag = 1;
	}
	else
	// �����{�^���u���v�����͂��ꂽ��J�����̕����Ɉړ�����
	if (input.IsPressed("Down"))
	{
		// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
		moveVec = VAdd(moveVec, VScale(upMoveVec, -1.0f));

		// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		moveFlag = 1;
	}

	// �v���C���[�̏�Ԃ��u�W�����v�v�ł͂Ȃ��A���{�^���P��������Ă�����W�����v����
	if (state_ != 2 && (input.IsTriggered("Jump")))
	{
		// ��Ԃ��u�W�����v�v�ɂ���
		state_ = 2;

		// �x�������̑��x���Z�b�g
		jumpPower_ = JUMP_POWER;

		// �W�����v�A�j���[�V�����̍Đ�
		ChangeAnim(PLAYER_ANIM_NAME::JUMP);
	}

	// �ړ��{�^���������ꂽ���ǂ����ŏ����𕪊�
	if (moveFlag)
	{
		// �ړ��x�N�g���𐳋K���������̂��v���C���[�������ׂ������Ƃ��ĕۑ�
		moveDirection_ = VNorm(moveVec);

		// �v���C���[�������ׂ������x�N�g�����v���C���[�̃X�s�[�h�{�������̂��ړ��x�N�g���Ƃ���
		moveVec = VScale(moveDirection_, MOVE_SPEED);

		// �������܂Łu�����~�܂�v��Ԃ�������
		if (state_ == 0)
		{
			ChangeAnim(PLAYER_ANIM_NAME::RUN);

			// ��Ԃ��u����v�ɂ���
			state_ = 1;
		}
	}
	else
	{
		// ���̃t���[���ňړ����Ă��Ȃ��āA����Ԃ��u����v��������
		if (state_ == 1)
		{
			// �����~��A�j���[�V�������Đ�����
			ChangeAnim(PLAYER_ANIM_NAME::IDLE);

			// ��Ԃ��u�����~��v�ɂ���
			state_ = 0;
		}
	}

	// ��Ԃ��u�W�����v�v�̏ꍇ��
	if (state_ == 2)
	{
		// �x�������̑��x���d�͕����Z����
		jumpPower_ -= GRAVITY;

		// �����������Ă��Ċ��Đ�����Ă���A�j���[�V�������㏸���p�̂��̂������ꍇ��
		if (jumpPower_ < 0.0f && MV1GetAttachAnim(modelHandle_, playAnim1_) == 2)
		{
			// �������悤�̃A�j���[�V�������Đ�����
			ChangeAnim(PLAYER_ANIM_NAME::FALL);
		}

		// �ړ��x�N�g���̂x�������x�������̑��x�ɂ���
		moveVec.y = jumpPower_;
	}

	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	UpdateAngle();

	// �ړ��x�N�g�������ɃR���W�������l�����v���C���[���ړ�
	Move(moveVec);

	// �A�j���[�V��������
	UpdateAnim();
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);

	DrawShadow();
}

void Player::IdleUpdate()
{
}

void Player::RunUpdate()
{
}

void Player::JumpUpdate()
{
}

void Player::Move(const VECTOR& moveVector)
{
	int i, j, k;						// �ėp�J�E���^�ϐ�
	int moveFlag;						// ���������Ɉړ��������ǂ����̃t���O( �O:�ړ����Ă��Ȃ�  �P:�ړ����� )
	int hitFlag;						// �|���S���ɓ����������ǂ������L�����Ă����̂Ɏg���ϐ�( �O:�������Ă��Ȃ�  �P:�������� )
	MV1_COLL_RESULT_POLY_DIM hitDim;			// �v���C���[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	int kabeNum;						// �ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	int yukaNum;						// ���|���S���Ɣ��f���ꂽ�|���S���̐�
	MV1_COLL_RESULT_POLY* kabe[MAX_HIT_COLLISION];	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* yuka[MAX_HIT_COLLISION];	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* poly;				// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^( �g��Ȃ��Ă��ς܂����܂����v���O�����������Ȃ�̂ŁE�E�E )
	HITRESULT_LINE lineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	VECTOR oldPos;						// �ړ��O�̍��W	
	VECTOR nowPos;						// �ړ���̍��W

	// �ړ��O�̍��W��ۑ�
	oldPos = pos_;

	// �ړ���̍��W���Z�o
	nowPos = VAdd(pos_, moveVector);

	// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
	// ( ���o����͈͈͂ړ��������l������ )
	hitDim = stage_.CollCheckSphere(pos_, COLLISION_SPHERE_SIZE + VSize(moveVector));

	// x����y�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
	if (fabs(moveVector.x) > 0.01f || fabs(moveVector.z) > 0.01f)
	{
		moveFlag = 1;
	}
	else
	{
		moveFlag = 0;
	}

	// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
	{
		// �ǃ|���S���Ə��|���S���̐�������������
		kabeNum = 0;
		yukaNum = 0;

		// ���o���ꂽ�|���S���̐������J��Ԃ�
		for (i = 0; i < hitDim.HitNum; i++)
		{
			// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
			if (hitDim.Dim[i].Normal.y < 0.000001f && hitDim.Dim[i].Normal.y > -0.000001f)
			{
				// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
				if (hitDim.Dim[i].Position[0].y > pos_.y + 1.0f ||
					hitDim.Dim[i].Position[1].y > pos_.y + 1.0f ||
					hitDim.Dim[i].Position[2].y > pos_.y + 1.0f)
				{
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (kabeNum < MAX_HIT_COLLISION)
					{
						// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
						kabe[kabeNum] = &hitDim.Dim[i];

						// �ǃ|���S���̐������Z����
						kabeNum++;
					}
				}
			}
			else
			{
				// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (yukaNum < MAX_HIT_COLLISION)
				{
					// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
					yuka[yukaNum] = &hitDim.Dim[i];

					// ���|���S���̐������Z����
					yukaNum++;
				}
			}
		}
	}

	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (kabeNum != 0)
	{
		// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
		hitFlag = 0;

		// �ړ��������ǂ����ŏ����𕪊�
		if (moveFlag == 1)
		{
			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < kabeNum; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				poly = kabe[i];

				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

				// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				hitFlag = 1;

				// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
				{
					VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(moveVector, poly->Normal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
					// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
					SlideVec = VCross(poly->Normal, SlideVec);

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					nowPos = VAdd(oldPos, SlideVec);
				}

				// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
				for (j = 0; j < kabeNum; j++)
				{
					// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly = kabe[j];

					// �������Ă����烋�[�v���甲����
					if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
				}

				// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				// �ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == kabeNum)
				{
					hitFlag = 0;
					break;
				}
			}
		}
		else
		{
			// �ړ����Ă��Ȃ��ꍇ�̏���

			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < kabeNum; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				poly = kabe[i];

				// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
				{
					hitFlag = 1;
					break;
				}
			}
		}

		// �ǂɓ������Ă�����ǂ��牟���o���������s��
		if (hitFlag == 1)
		{
			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
			for (k = 0; k < HIT_TRY_NUM; k++)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < kabeNum; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					poly = kabe[i];

					// �v���C���[�Ɠ������Ă��邩�𔻒�
					if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
					nowPos = VAdd(nowPos, VScale(poly->Normal, HIT_SLIDE_LENGTH));

					// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					for (j = 0; j < kabeNum; j++)
					{
						// �������Ă����烋�[�v�𔲂���
						poly = kabe[j];
						if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
					}

					// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
					if (j == kabeNum) break;
				}

				// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
				if (i != kabeNum) break;
			}
		}
	}

	// ���|���S���Ƃ̓����蔻��
	if (yukaNum != 0)
	{
		// �W�����v�����㏸���̏ꍇ�͏����𕪊�
		if (state_ == 2 && jumpPower_ > 0.0f)
		{
			float MinY;

			// �V��ɓ����Ԃ��鏈�����s��

			// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
			MinY = 0.0f;

			// �����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���
			hitFlag = 0;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < yukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				poly = yuka[i];

				// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				lineRes = HitCheck_Line_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);

				// �ڐG���Ă��Ȃ������牽�����Ȃ�
				if (lineRes.HitFlag == FALSE) continue;

				// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
				if (hitFlag == 1 && MinY < lineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				hitFlag = 1;

				// �ڐG�����x���W��ۑ�����
				MinY = lineRes.Position.y;
			}

			// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
			if (hitFlag == 1)
			{
				// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
				nowPos.y = MinY - HIT_HEIGHT;

				// �x�������̑��x�͔��]
				jumpPower_ = -jumpPower_;
			}
		}
		else
		{
			float MaxY;

			// ���~�����W�����v���ł͂Ȃ��ꍇ�̏���

			// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			hitFlag = 0;

			// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			MaxY = 0.0f;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < yukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				poly = yuka[i];

				// �W�����v�����ǂ����ŏ����𕪊�
				if (state_ == 2)
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
				if (lineRes.HitFlag == FALSE) continue;

				// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (hitFlag == 1 && MaxY > lineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				hitFlag = 1;

				// �ڐG�����x���W��ۑ�����
				MaxY = lineRes.Position.y;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (hitFlag == 1)
			{
				// ���������ꍇ

				// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				nowPos.y = MaxY;

				// �x�������̈ړ����x�͂O��
				jumpPower_ = 0.0f;

				// �����W�����v���������ꍇ�͒��n��Ԃɂ���
				if (state_ == 2)
				{
					// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
					if (moveFlag)
					{
						// �ړ����Ă���ꍇ�͑����Ԃ�
						ChangeAnim(PLAYER_ANIM_NAME::RUN);
						state_ = 1;
					}
					else
					{
						// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
						ChangeAnim(PLAYER_ANIM_NAME::IDLE);
						state_ = 0;
					}

					// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
					animBlendRate_ = 1.0f;
				}
			}
			else
			{
				// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
				if (state_ != 2)
				{
					// �W�����v���ɂ���
					state_ = 2;

					// ������Ƃ����W�����v����
					jumpPower_ = FALL_UP_POWER;

					// �A�j���[�V�����͗������̂��̂ɂ���
					ChangeAnim(PLAYER_ANIM_NAME::FALL);
				}
			}
		}
	}

	// �V�������W��ۑ�����
	pos_ = nowPos;

	// �v���C���[�̃��f���̍��W���X�V����
	MV1SetPosition(modelHandle_, pos_);

	// ���o�����v���C���[�̎��͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(hitDim);

}

void Player::UpdateAngle()
{
	float targetAngle;			// �ڕW�p�x
	float diffAngle;			// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	targetAngle = atan2(moveDirection_.x, moveDirection_.z);

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	{
		// �ŏ��͒P���Ɉ����Z
		diffAngle = targetAngle - angle_;

		// ����������炠������̍����P�W�O�x�ȏ�ɂȂ邱�Ƃ͖����̂�
		// ���̒l���P�W�O�x�ȏ�ɂȂ��Ă�����C������
		if (diffAngle < -DX_PI_F)
		{
			diffAngle += DX_TWO_PI_F;
		}
		else
			if (diffAngle > DX_PI_F)
			{
				diffAngle -= DX_TWO_PI_F;
			}
	}

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

void Player::ChangeAnim(PLAYER_ANIM_NAME playAnim)
{
	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (playAnim2_ != -1)
	{
		MV1DetachAnim(modelHandle_, playAnim2_);
		playAnim2_ = -1;
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	playAnim2_ = playAnim1_;
	animPlayCount2_ = animPlayCount1_;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	playAnim1_ = MV1AttachAnim(modelHandle_, static_cast<int>(playAnim));
	animPlayCount1_ = 0.0f;

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	animBlendRate_ = playAnim2_ == -1 ? 1.0f : 0.0f;
}

void Player::UpdateAnim()
{
	float totalTime;

	UpdateAnimBlendRate();

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (playAnim1_ != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		totalTime = MV1GetAttachAnimTotalTime(modelHandle_, playAnim1_);

		// �Đ����Ԃ�i�߂�
		animPlayCount1_ += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (animPlayCount1_ >= totalTime)
		{
			animPlayCount1_ = fmod(animPlayCount1_, totalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(modelHandle_, playAnim1_, animPlayCount1_);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(modelHandle_, playAnim1_, animBlendRate_);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (playAnim2_ != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		totalTime = MV1GetAttachAnimTotalTime(modelHandle_, playAnim2_);

		// �Đ����Ԃ�i�߂�
		animPlayCount2_ += ANIM_PLAY_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (animPlayCount2_ > totalTime)
		{
			animPlayCount2_ = fmod(animPlayCount2_, totalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(modelHandle_, playAnim2_, animPlayCount2_);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(modelHandle_, playAnim2_, 1.0f - animBlendRate_);
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

void Player::DrawShadow()
{
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitResDim = stage_.CollCheckCapsule(pos_, VAdd(pos_, VGet(0.0f, -SHADOW_HEIGHT, 0.0f)), SHADOW_SIZE);

	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
	{
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

		// �|���S���̕s�����x��ݒ肷��
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > pos_.y - SHADOW_HEIGHT)
			Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - pos_.y) / SHADOW_HEIGHT);

		if (HitRes->Position[1].y > pos_.y - SHADOW_HEIGHT)
			Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - pos_.y) / SHADOW_HEIGHT);

		if (HitRes->Position[2].y > pos_.y - SHADOW_HEIGHT)
			Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - pos_.y) / SHADOW_HEIGHT);

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		Vertex[0].u = (HitRes->Position[0].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(Vertex, 1, shadowHandle_, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(HitResDim);

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	// �y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(FALSE);

}
