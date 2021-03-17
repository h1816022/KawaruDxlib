#pragma once
#include <unordered_map>

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

class AnimationComponent
{
public:
    AnimationComponent(int modelHandle);
    ~AnimationComponent();

    /// <summary>
    /// �X�V
    /// </summary>
    void Update();

    /// <summary>
    /// �A�j���[�V�����̕ύX
    /// </summary>
    /// <param name="playAnim">�A�j���[�V������</param>
    void ChangeAnim(ANIM_NAME playAnim);

    /// <summary>
    /// �Đ����̃A�j���[�V�����ƈ�v���邩
    /// </summary>
    /// <param name="target">���ׂ����A�j���[�V������</param>
    /// <returns>true : ��v����</returns>
    bool CheckNowAnim(ANIM_NAME target)const;

    /// <summary>
    /// �A�j���[�V�������u�����h����1.0f�ɂ��A�����I�ɑJ�ڂ�����������
    /// </summary>
    void ForceBlend();

    int GetNowAnimTotalTime();

    bool EndAnim();

private:
    void InitAnimData();

    /// <summary>
    /// �A�j���[�V�����̃u�����h�󋵂��X�V
    /// </summary>
    void UpdateAnimBlendRate();

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

    int animAttachNum1_ = -1;	    // �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�(-1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ�)
    float animPlayCount1_ = 0.0f;	// �Đ����Ă���A�j���[�V�����P�̍Đ�����
    int animAttachNum2_ = -1;	    // �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�(-1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ�)
    float animPlayCount2_ = 0.0f;	// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
    float animBlendRate_ = 0.0f;	// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��

    std::unordered_map<ANIM_NAME, AnimData> animData_;

    int modelHandle_;
};

