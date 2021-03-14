#pragma once
#include <memory>
#include "Actor.h"
#include "Players/Player.h"

class Stage;

enum class CAMERA_MODE
{
    PlayerFollow,   // �v���C���[���f��
    TargetFollow,   // �^�[�Q�b�g���f��
    Manual          // �^�[�Q�b�g���w�肵�Ȃ�
};

struct Angle
{
    float horizontal;
    float vertical;
};

class Camera :
    public Actor
{
public:
    Camera(const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Camera();

    void Update(const Input& input)override final;

    void Draw()override final;

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

    void ChangeMode(CAMERA_MODE mode);

    bool GetFollowingPlayerFlag();

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

    Angle CalcAngle(const VECTOR& nowVec, const VECTOR& targetVec);

    /// <summary>
    /// �J�����A�[���̍X�V
    /// </summary>
    /// <param name="rotY">Y��]</param>
    /// <param name="rotZ">Z��]</param>
    void UpdateArmLength(const MATRIX& rotY, const MATRIX& rotZ);

    void LostPlayer();

    void UpdatePlayerFollowMode();

    void UpdateTargetFollowMode();
    
    void UpdateManualMode();

    /// <summary>
    /// �v���C���[�������Ă��邩
    /// </summary>
    /// <returns>true : �����Ă���</returns>
    bool CanSeePlayer();

    using UpdateFunc = void (Camera::*)();
    UpdateFunc updaterByMode_;

    VECTOR targetPos_;

    Angle angle_;

    std::shared_ptr<Actor> targetActor_;

    std::shared_ptr<Player> player_;

    const Stage& stage_;

    VECTOR setEye_;
    VECTOR setTarget_;
    
    float targetHeightOffset_;

    float armLength_;

    bool followingPlayer_ = false;

    CAMERA_MODE mode_ = CAMERA_MODE::PlayerFollow;

    VECTOR nowShakeOffset_;
    VECTOR targetShakeOffset_;

    int shakeCount_ = 0;
};