#pragma once
#include <memory>
#include "Actor.h"
#include "Players/Player.h"

class Stage;

// �J�����̓����̃��[�h
enum class CAMERA_MODE
{
    PlayerFollow,   // �v���C���[���f��
    TargetFollow,   // �^�[�Q�b�g���f��
    GameEnd         // 1�v���C�I����
};

// �p�x���
struct Angle
{
    float horizontal;   // �����p�x
    float vertical;     // �����p�x
};

/// <summary>
/// �J�����֘A�̃N���X
/// </summary>
class Camera :
    public Actor
{
public:
    Camera(Scene& scene, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Camera();

    /// <summary>
    /// �X�V����
    /// </summary>
    /// <param name="input">���͏��</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// �J�����̃u�������X�V
    /// </summary>
    void UpdateCameraShake();

    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw()override final;

    /// <summary>
    /// ���W�̃Z�b�g
    /// </summary>
    /// <param name="pos">���W</param>
    void SetPos(const VECTOR& pos);

    /// <summary>
    /// �����_�̍��W���擾
    /// </summary>
    /// <returns>�����_�̍��W</returns>
    const VECTOR& GetTargetPos()const;

    /// <summary>
    /// �����_�̊�Ƃ���A�N�^�[���Z�b�g
    /// </summary>
    /// <param name="target">�����_�̊</param>
    void SetTargetActor(std::shared_ptr<Actor> target);

    /// <summary>
    /// �v���C���[�̃|�C���^���Z�b�g
    /// </summary>
    /// <param name="player">�v���C���[</param>
    void SetPlayer(std::shared_ptr<Player> player);

    /// <summary>
    /// �v���C���[�������Ă���Ȃ�A�����Ă��镔���̍��W���擾
    /// </summary>
    /// <param name="visiblePos">���������W</param>
    /// <returns>true : �����Ă���</returns>
    bool GetVisiblePlayerPos(VECTOR& visiblePos);

    /// <summary>
    /// ���[�h�̕ύX
    /// </summary>
    /// <param name="mode">�ύX���郂�[�h</param>
    void ChangeMode(CAMERA_MODE mode);

    /// <summary>
    /// �v���C���[��Ǐ]���Ă��邩
    /// </summary>
    /// <returns>true : �Ǐ]���Ă���</returns>
    bool GetFollowingPlayerFlag();

    /// <summary>
    /// 1�Q�[�����I��������̏���
    /// </summary>
    void EndGame();

private:
    /// <summary>
    /// �p�x�X�V
    /// </summary>
    /// <param name="input">���͏��</param>
    void UpdateAngle(const Input& input);

    /// <summary>
    /// ���W(���_)�X�V
    /// </summary>
    void UpdatePos();

    /// <summary>
    /// �c��񂾊p�x�𓯓��̐��l�ɐ�����
    /// </summary>
    void ClampAngle();

    /// <summary>
    /// �p�x���v�Z���ĕԂ�
    /// </summary>
    /// <param name="nowVec">���݂̌����x�N�g��</param>
    /// <param name="targetVec">�ڕW�̌����x�N�g��</param>
    /// <returns>�ڕW�ւ̊p�x</returns>
    Angle CalcAngle(const VECTOR& nowVec, const VECTOR& targetVec);

    /// <summary>
    /// �J�����A�[���̍X�V
    /// </summary>
    /// <param name="rotY">Y��]</param>
    /// <param name="rotZ">Z��]</param>
    void UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ);

    /// <summary>
    /// �v���C���[�������������̏���
    /// </summary>
    void LostPlayer();

    /// <summary>
    /// �v���C���[�Ǐ]���[�h�̂Ƃ��̍X�V����
    /// </summary>
    void UpdatePlayerFollowMode();

    /// <summary>
    /// �w��^�[�Q�b�g�A�N�^�[�Ǐ]���[�h�̂Ƃ��̍X�V����
    /// </summary>
    void UpdateTargetFollowMode();
    
    /// <summary>
    /// �Q�[���N���A�����Ƃ��̍X�V����
    /// </summary>
    void UpdateGameClear();

    /// <summary>
    /// �v���C���[�������Ă��邩
    /// </summary>
    /// <returns>true : �����Ă���</returns>
    bool CanSeePlayer();

    using UpdateFunc = void (Camera::*)();
    UpdateFunc updaterByMode_;

    // �����_
    VECTOR targetPos_;

    // �J�����̊p�x
    Angle angle_;

    // ��������A�N�^�[
    std::shared_ptr<Actor> targetActor_;

    std::shared_ptr<Player> player_;

    const Stage& stage_;

    // �b��̎��_���W
    VECTOR setEye_;

    // �b��̒����_���W
    VECTOR setTarget_;
    
    /// <summary>
    /// �����_�̎Z�o���AtargetActor�̍��W����AY�������ɑ����I�t�Z�b�g�l
    /// </summary>
    float targetHeightOffset_;

    /// <summary>
    /// �J�����A�[���̒���
    /// </summary>
    float armLength_;
    
    // �v���C���[��Ǐ]���Ă��邩
    bool followingPlayer_ = false;

    CAMERA_MODE mode_ = CAMERA_MODE::PlayerFollow;

    // ���݂̎�u���̃I�t�Z�b�g�l
    VECTOR nowShakeOffset_;

    // �ڕW�̎�u���̃I�t�Z�b�g�l
    VECTOR targetShakeOffset_;

    // ��u���v�Z�p�J�E���g
    int shakeCount_ = 0;
};