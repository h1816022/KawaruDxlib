#pragma once
#include <string>
#include "../Actor.h"

class Camera;
class Stage;

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
    using UpdateFunc = void (Player::*)();
    UpdateFunc updater_;

    void IdleUpdate();
    void RunUpdate();
    void JumpUpdate();

    void Move(const VECTOR& moveVector);

    void UpdateAngle();

    /// <summary>
    /// �A�j���[�V�����̕ύX
    /// </summary>
    /// <param name="playAnim">�A�j���[�V�����̖��O</param>
    void ChangeAnim(PLAYER_ANIM_NAME playAnim);

    /// <summary>
    /// �A�j���[�V�������X�V
    /// </summary>
    void UpdateAnim();

    /// <summary>
    /// �A�j���[�V�����̃u�����h�󋵂��X�V
    /// </summary>
    void UpdateAnimBlendRate();

    void DrawShadow();

    float jumpPower_ = 0.0f;

    VECTOR moveDirection_;

    const Camera& camera_;

    const Stage& stage_;

    int state_ = 0;

    int playAnim1_;				// �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
    float animPlayCount1_;			// �Đ����Ă���A�j���[�V�����P�̍Đ�����
    int playAnim2_;				// �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
    float animPlayCount2_;			// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
    float animBlendRate_;				// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��

    int shadowHandle_;
};