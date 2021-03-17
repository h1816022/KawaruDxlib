#pragma once
#include <unordered_map>

// アニメーションの名前
enum class ANIM_NAME : int
{
    Invalid = -1,   // 無効
    Idle = 0,       // 待機
    Walk = 1,       // 歩き
    Jump = 2,       // ジャンプ
    Fall = 3,       // 落下
    Dead = 4,       // やられ
    Call = 5,       // 呼ぶ
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
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// アニメーションの変更
    /// </summary>
    /// <param name="playAnim">アニメーション名</param>
    void ChangeAnim(ANIM_NAME playAnim);

    /// <summary>
    /// 再生中のアニメーションと一致するか
    /// </summary>
    /// <param name="target">調べたいアニメーション名</param>
    /// <returns>true : 一致した</returns>
    bool CheckNowAnim(ANIM_NAME target)const;

    /// <summary>
    /// アニメーションをブレンド率を1.0fにし、強制的に遷移を完了させる
    /// </summary>
    void ForceBlend();

    int GetNowAnimTotalTime();

    bool EndAnim();

private:
    void InitAnimData();

    /// <summary>
    /// アニメーションのブレンド状況を更新
    /// </summary>
    void UpdateAnimBlendRate();

    /// <summary>
    /// アニメーションのアタッチ
    /// </summary>
    /// <param name="animName">アタッチするアニメーション名</param>
    /// <returns>アタッチ番号</returns>
    int AttachAnim(ANIM_NAME animName);

    /// <summary>
    /// アニメーションのデタッチ
    /// </summary>
    /// <param name="animAttachNum">アニメーションアタッチ番号</param>
    void DetachAnim(int animAttachNum);

    /// <summary>
    /// 指定アニメーションの総再生時間を取得
    /// </summary>
    /// <param name="animattachNum">アニメーションアタッチ番号</param>
    /// <returns>総再生時間</returns>
    float GetAnimTotalTime(int animattachNum);

    void SetAnimTime(int animAttachNum, int count);

    void SetAnimBlendRate(int animAttachNum, float rate);

    AnimData GetAnimData(int animAttachNum);

    void UpdateAnimCount(int animAttachNum, float& animCount);

    int animAttachNum1_ = -1;	    // 再生しているアニメーション１のアタッチ番号(-1:何もアニメーションがアタッチされていない)
    float animPlayCount1_ = 0.0f;	// 再生しているアニメーション１の再生時間
    int animAttachNum2_ = -1;	    // 再生しているアニメーション２のアタッチ番号(-1:何もアニメーションがアタッチされていない)
    float animPlayCount2_ = 0.0f;	// 再生しているアニメーション２の再生時間
    float animBlendRate_ = 0.0f;	// 再生しているアニメーション１と２のブレンド率

    std::unordered_map<ANIM_NAME, AnimData> animData_;

    int modelHandle_;
};

