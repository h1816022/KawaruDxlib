#pragma once
#include <string>
#include "../Actor.h"

class Camera;
class Stage;
struct HitCheckPolyData;

// �A�j���[�V�����̖��O
enum class PLAYER_ANIM_NAME : int
{
    INVALID = -1,   // ����
    WALK = 0,       // ����
    RUN = 1,        // ����
    JUMP = 2,       // �W�����v
    FALL = 3,       // ����
    IDLE = 4        // �ҋ@
};

class Player :
    public Actor
{
public:
    Player(const Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Player();

    void Update(const Input& input)override final;

    void Draw()override final;

private:
    using UpdateFunc = void (Player::*)(const Input& input);
    UpdateFunc updater_;

    void IdleUpdate(const Input& input);
    void RunUpdate(const Input& input);
    void JumpUpdate(const Input& input);

    void Jump();

    void Move(const VECTOR& moveVector);

    void CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec);

    bool CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input);

    void UpdateAngle();

    float CalcAngleDiff(float target)const;

    /// <summary>
    /// �A�j���[�V�����̕ύX
    /// </summary>
    /// <param name="playAnim">�A�j���[�V������</param>
    void ChangeAnim(PLAYER_ANIM_NAME playAnim);

    /// <summary>
    /// �A�j���[�V�������X�V
    /// </summary>
    void UpdateAnim();

    /// <summary>
    /// �A�j���[�V�����̃u�����h�󋵂��X�V
    /// </summary>
    void UpdateAnimBlendRate();

    /// <summary>
    /// �Đ����̃A�j���[�V�����ƈ�v���邩
    /// </summary>
    /// <param name="animAttachNum">�A�j���[�V�����A�^�b�`�ԍ�</param>
    /// <param name="target">���ׂ����A�j���[�V������</param>
    /// <returns>true : ��v����</returns>
    bool CheckNowAnim(int animAttachNum, PLAYER_ANIM_NAME target)const;

    /// <summary>
    /// �A�j���[�V�����̃A�^�b�`
    /// </summary>
    /// <param name="animName">�A�^�b�`����A�j���[�V������</param>
    /// <returns>�A�^�b�`�ԍ�</returns>
    int AttachAnim(PLAYER_ANIM_NAME animName);

    /// <summary>
    /// �A�j���[�V�����̃f�^�b�`
    /// </summary>
    /// <param name="animAttachNum">�A�j���[�V�����A�^�b�`�ԍ�</param>
    void DetachAnim(int animAttachNum);

    /// <summary>
    /// �w��A�j���[�V�����̑��Đ����Ԃ��擾
    /// </summary>
    /// <param name="animattachNum">�A�j���[�V�����A�^�b�`�ԍ�</param>
    /// <returns>���Đ�����</returns>
    float GetAnimTotalTime(int animattachNum);

    void SetAnimTime(int animAttachNum, int count);

    void SetAnimBlendRate(int animAttachNum, float rate);

    void DrawShadow();

    void GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData);

    bool CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos);

    void CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos);

    /// <summary>
    /// �����o��
    /// </summary>
    /// <param name="polyData">���肷��|���S���f�[�^</param>
    /// <param name="nowPos">���ݍ��W</param>
    void Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos);

    float jumpPower_ = 0.0f;

    VECTOR moveDirection_;

    VECTOR moveVec;

    const Camera& camera_;

    const Stage& stage_;

    int animAttachNum1_ = -1;	// �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
    float animPlayCount1_;			// �Đ����Ă���A�j���[�V�����P�̍Đ�����
    int animAttachNum2_ = -1;	// �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
    float animPlayCount2_;			// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
    float animBlendRate_;			// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��

    int shadowHandle_;
};