#pragma once
#include <unordered_map>
#include "Actor.h"

class Input;
class Stage;
struct HitCheckPolyData;

// �A�j���[�V�����̖��O
enum class ANIM_NAME : int
{
    Invalid = -1,   // ����
    Idle = 0,       // �ҋ@
    Walk = 1,       // ����
    Jump = 2,       // �W�����v
    Fall = 3,       // ����
    Dead = 4,       // ����
    Call = 5,       // �Ă�
    Max
};

struct AnimData
{
    bool isLoop;
    int totalTime;
};

enum class UPDATE_TYPE
{
    Idle,
    Run,
    Jump
};

class Character :
    public Actor
{
public:
    Character(const Stage& stage, const float hitWidth, const float hitHeight,const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);

    Character(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const Stage& stage, const float hitWidth, const float hitHeight, 
        const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    virtual ~Character();

    virtual void Update(const Input& input) = 0;
    virtual void Draw() = 0;

    void ChangeUpadater(UPDATE_TYPE type);

protected:
    void UpdatePos(const VECTOR& moveVector);

    void GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData);

    bool CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos);

    void CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos);

    void Jump();

    int GetNowAnimTotalTime();

    /// <summary>
    /// �A�j���[�V�������X�V
    /// </summary>
    void UpdateAnim();

    /// <summary>
    /// �A�j���[�V�����̕ύX
    /// </summary>
    /// <param name="playAnim">�A�j���[�V������</param>
    void ChangeAnim(ANIM_NAME playAnim);

    /// <summary>
    /// �����o��
    /// </summary>
    /// <param name="polyData">���肷��|���S���f�[�^</param>
    /// <param name="nowPos">���ݍ��W</param>
    void Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos);

    void DrawShadow();

    void StopMove(float time);

    using UpdateFunc = void (Character::*)(const Input& input);
    UpdateFunc updater_;

    virtual void IdleUpdate(const Input& input);
    virtual void RunUpdate(const Input& input);
    virtual void JumpUpdate(const Input& input);

    void UpdateAngle();

    // �����蔻��J�v�Z���̔��a
    const float hitWidth_;

    // �����蔻��J�v�Z���̍���
    const float hitHeight_;

    bool moveFlag_ = false;

    VECTOR moveVec_;

    VECTOR moveDirection_;

    const Stage& stage_;

    bool canMove_ = true;

private:
    void InitAnimData();

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
    bool CheckNowAnim(int animAttachNum, ANIM_NAME target)const;

    /// <summary>
    /// �A�j���[�V�����̃A�^�b�`
    /// </summary>
    /// <param name="animName">�A�^�b�`����A�j���[�V������</param>
    /// <returns>�A�^�b�`�ԍ�</returns>
    int AttachAnim(ANIM_NAME animName);

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

    AnimData GetAnimData(int animAttachNum);

    void UpdateAnimCount(int animAttachNum, float& animCount);

    float CalcAngleDiff(float target)const;

    void UpdateMoveStopTime();

    int shadowHandle_;

    float jumpPower_ = 0.0f;

    int animAttachNum1_ = -1;	    // �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�(-1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ�)
    float animPlayCount1_ = 0.0f;	// �Đ����Ă���A�j���[�V�����P�̍Đ�����
    int animAttachNum2_ = -1;	    // �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�(-1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ�)
    float animPlayCount2_ = 0.0f;	// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
    float animBlendRate_ = 0.0f;	// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��

    UPDATE_TYPE nowUpdateType_ = UPDATE_TYPE::Idle;

    std::unordered_map<ANIM_NAME, AnimData> animData_;

    int nowMoveStopTime_ = 0;
    int moveStopTime_ = 0;
};