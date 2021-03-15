#pragma once
#include <unordered_map>
#include "Actor.h"

class Input;
class Stage;
struct HitCheckPolyData;

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
    /// アニメーションを更新
    /// </summary>
    void UpdateAnim();

    /// <summary>
    /// アニメーションの変更
    /// </summary>
    /// <param name="playAnim">アニメーション名</param>
    void ChangeAnim(ANIM_NAME playAnim);

    /// <summary>
    /// 押し出し
    /// </summary>
    /// <param name="polyData">判定するポリゴンデータ</param>
    /// <param name="nowPos">現在座標</param>
    void Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos);

    void DrawShadow();

    void StopMove(float time);

    using UpdateFunc = void (Character::*)(const Input& input);
    UpdateFunc updater_;

    virtual void IdleUpdate(const Input& input);
    virtual void RunUpdate(const Input& input);
    virtual void JumpUpdate(const Input& input);

    void UpdateAngle();

    // 当たり判定カプセルの半径
    const float hitWidth_;

    // 当たり判定カプセルの高さ
    const float hitHeight_;

    bool moveFlag_ = false;

    VECTOR moveVec_;

    VECTOR moveDirection_;

    const Stage& stage_;

    bool canMove_ = true;

private:
    void InitAnimData();

    /// <summary>
    /// アニメーションのブレンド状況を更新
    /// </summary>
    void UpdateAnimBlendRate();

    /// <summary>
    /// 再生中のアニメーションと一致するか
    /// </summary>
    /// <param name="animAttachNum">アニメーションアタッチ番号</param>
    /// <param name="target">調べたいアニメーション名</param>
    /// <returns>true : 一致した</returns>
    bool CheckNowAnim(int animAttachNum, ANIM_NAME target)const;

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

    float CalcAngleDiff(float target)const;

    void UpdateMoveStopTime();

    int shadowHandle_;

    float jumpPower_ = 0.0f;

    int animAttachNum1_ = -1;	    // 再生しているアニメーション１のアタッチ番号(-1:何もアニメーションがアタッチされていない)
    float animPlayCount1_ = 0.0f;	// 再生しているアニメーション１の再生時間
    int animAttachNum2_ = -1;	    // 再生しているアニメーション２のアタッチ番号(-1:何もアニメーションがアタッチされていない)
    float animPlayCount2_ = 0.0f;	// 再生しているアニメーション２の再生時間
    float animBlendRate_ = 0.0f;	// 再生しているアニメーション１と２のブレンド率

    UPDATE_TYPE nowUpdateType_ = UPDATE_TYPE::Idle;

    std::unordered_map<ANIM_NAME, AnimData> animData_;

    int nowMoveStopTime_ = 0;
    int moveStopTime_ = 0;
};