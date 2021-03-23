#pragma once
#include <memory>
#include "../Tools/Geometry.h"
#include "Actor.h"

class Input;
class Stage;
class AnimationComponent;

struct HitCheckPolyData;

// Update�̎��
enum class UPDATE_TYPE
{
    Idle,   // �ҋ@
    Run,    // �ړ�
    Jump,   // �W�����v
    Destroy // ���S��
};

/// <summary>
/// �A�N�^�[�̒��ŁA�v���C���[���͂��߃L�����N�^�[�Ƃ��ĐU�镑���A�N�^�[�̊��N���X
/// </summary>
class Character :
    public Actor
{
public:
    Character(Scene& scene, const Stage& stage, const float hitWidth, const float hitHeight,const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);

    Character(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const Stage& stage, const float hitWidth, const float hitHeight, 
        const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    virtual ~Character();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void Update(const Input& input) = 0;

    /// <summary>
    /// �`�揈��
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// �����蔻��p�J�v�Z���̎擾
    /// </summary>
    /// <param name="pos">���ݍ��W</param>
    /// <returns>�����蔻��p�J�v�Z��</returns>
    Capsule3D GetCollisionCapsule(const VECTOR& pos)override;

    /// <summary>
    /// ���̃A�N�^�[������
    /// </summary>
    /// <returns>true : ������ false : ���ɏ����邱�ƂɂȂ��Ă��邩�A�����Ȃ�����</returns>
    virtual bool Destroy()override;

    /// <summary>
    /// ���݂̍X�V�^�C�v���擾
    /// </summary>
    /// <returns>�X�V�^�C�v</returns>
    const UPDATE_TYPE& GetNowUpdateType()const;

protected:
    /// <summary>
    /// �X�V�^�C�v��ύX
    /// </summary>
    /// <param name="type">�V�����^�C�v</param>
    void ChangeUpadater(UPDATE_TYPE type);

    /// <summary>
    /// ���W�̍X�V
    /// </summary>
    /// <param name="moveVector">�ړ��x�N�g��</param>
    void UpdatePos(const VECTOR& moveVector);

    /// <summary>
    /// �w��|���S���̂����A�ǂɂ�������̂����ɂ�������̂���ʂ��擾
    /// </summary>
    /// <param name="outPolyData">�ǂ��������肵���|���S���̃��X�g</param>
    /// <param name="HitData">���肷��|���S��</param>
    void GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData);

    /// <summary>
    /// �ǃ|���S���Ƃ̓����蔻��
    /// </summary>
    /// <param name="moveFlag">���̃t���[���ړ����Ă��邩</param>
    /// <param name="polyData">�X�e�[�W�|���S���̃f�[�^</param>
    /// <param name="moveVector">�ړ��x�N�g��</param>
    /// <param name="nowPos">�b����W</param>
    /// <returns>true : �������Ă���</returns>
    bool CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos);

    /// <summary>
    /// ���|���S���Ƃ̓����蔻��
    /// </summary>
    /// <param name="moveFlag">���̃t���[���ړ����Ă��邩</param>
    /// <param name="polyData">�X�e�[�W�|���S���̃f�[�^</param>
    /// <param name="nowPos">�b����W</param>
    /// <returns>true : �������Ă���</returns>
    void CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos);

    /// <summary>
    /// �W�����v������
    /// </summary>
    void Jump();

    /// <summary>
    /// �����o��
    /// </summary>
    /// <param name="polyData">���肷��|���S���f�[�^</param>
    /// <param name="nowPos">���ݍ��W</param>
    void Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos);

    /// <summary>
    /// �e�̕`��
    /// </summary>
    void DrawShadow();

    /// <summary>
    /// �w�莞�ԁA�ړ��ł��Ȃ��悤�ɂ���
    /// </summary>
    /// <param name="time">�ړ������Ȃ�����</param>
    void StopMove(float time);

    using UpdateFunc = void (Character::*)(const Input& input);
    UpdateFunc updater_;

    /// <summary>
    /// �ҋ@���̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void IdleUpdate(const Input& input);

    /// <summary>
    /// ���蒆�̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void RunUpdate(const Input& input);

    /// <summary>
    /// �W�����v���̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void JumpUpdate(const Input& input);

    /// <summary>
    /// ���S���̍X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    virtual void DestroyUpdate(const Input& input);
    
    /// <summary>
    /// ��]�p�x�̍X�V
    /// </summary>
    void UpdateAngle();

    // �����蔻��J�v�Z���̔��a
    const float hitWidth_;

    // �����蔻��J�v�Z���̍���
    const float hitHeight_;

    bool moveFlag_ = false;

    bool canMove_ = true;

    bool collisionEnabled_ = true;

    VECTOR moveVec_;

    VECTOR moveDirection_;

    const Stage& stage_;

    std::unique_ptr<AnimationComponent> animationComponent_;

private:
    /// <summary>
    /// �p�x�̍����擾
    /// </summary>
    /// <param name="target">�ڕW�̊p�x</param>
    /// <returns>�p�x�̍�</returns>
    float CalcAngleDiff(float target)const;

    /// <summary>
    /// �ړ���~���Ԃ̍X�V
    /// </summary>
    void UpdateMoveStopTime();

    int shadowHandle_;

    float jumpPower_ = 0.0f;

    UPDATE_TYPE nowUpdateType_ = UPDATE_TYPE::Idle;

    int nowMoveStopTime_ = 0;
    int moveStopTime_ = 0;

    int jumpSE_ = -1;
};