#pragma once
#include <memory>
#include "../Tools/Geometry.h"
#include "Actor.h"

class Input;
class Stage;
class AnimationComponent;

struct HitCheckPolyData;

// Updateの種類
enum class UPDATE_TYPE
{
    Idle,   // 待機
    Run,    // 移動
    Jump,   // ジャンプ
    Destroy // 死亡時
};

/// <summary>
/// アクターの中で、プレイヤーをはじめキャラクターとして振る舞うアクターの基底クラス
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
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void Update(const Input& input) = 0;

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// 当たり判定用カプセルの取得
    /// </summary>
    /// <param name="pos">現在座標</param>
    /// <returns>当たり判定用カプセル</returns>
    Capsule3D GetCollisionCapsule(const VECTOR& pos)override;

    /// <summary>
    /// このアクターを消す
    /// </summary>
    /// <returns>true : 消せた false : 既に消えることになっているか、消せなかった</returns>
    virtual bool Destroy()override;

    /// <summary>
    /// 現在の更新タイプを取得
    /// </summary>
    /// <returns>更新タイプ</returns>
    const UPDATE_TYPE& GetNowUpdateType()const;

protected:
    /// <summary>
    /// 更新タイプを変更
    /// </summary>
    /// <param name="type">新しいタイプ</param>
    void ChangeUpadater(UPDATE_TYPE type);

    /// <summary>
    /// 座標の更新
    /// </summary>
    /// <param name="moveVector">移動ベクトル</param>
    void UpdatePos(const VECTOR& moveVector);

    /// <summary>
    /// 指定ポリゴンのうち、壁にあたるものか床にあたるものか区別し取得
    /// </summary>
    /// <param name="outPolyData">壁か床か判定したポリゴンのリスト</param>
    /// <param name="HitData">判定するポリゴン</param>
    void GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData);

    /// <summary>
    /// 壁ポリゴンとの当たり判定
    /// </summary>
    /// <param name="moveFlag">このフレーム移動しているか</param>
    /// <param name="polyData">ステージポリゴンのデータ</param>
    /// <param name="moveVector">移動ベクトル</param>
    /// <param name="nowPos">暫定座標</param>
    /// <returns>true : 当たっている</returns>
    bool CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos);

    /// <summary>
    /// 床ポリゴンとの当たり判定
    /// </summary>
    /// <param name="moveFlag">このフレーム移動しているか</param>
    /// <param name="polyData">ステージポリゴンのデータ</param>
    /// <param name="nowPos">暫定座標</param>
    /// <returns>true : 当たっている</returns>
    void CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos);

    /// <summary>
    /// ジャンプさせる
    /// </summary>
    void Jump();

    /// <summary>
    /// 押し出し
    /// </summary>
    /// <param name="polyData">判定するポリゴンデータ</param>
    /// <param name="nowPos">現在座標</param>
    void Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos);

    /// <summary>
    /// 影の描画
    /// </summary>
    void DrawShadow();

    /// <summary>
    /// 指定時間、移動できないようにする
    /// </summary>
    /// <param name="time">移動させない時間</param>
    void StopMove(float time);

    using UpdateFunc = void (Character::*)(const Input& input);
    UpdateFunc updater_;

    /// <summary>
    /// 待機中の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void IdleUpdate(const Input& input);

    /// <summary>
    /// 走り中の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void RunUpdate(const Input& input);

    /// <summary>
    /// ジャンプ中の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void JumpUpdate(const Input& input);

    /// <summary>
    /// 死亡中の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void DestroyUpdate(const Input& input);
    
    /// <summary>
    /// 回転角度の更新
    /// </summary>
    void UpdateAngle();

    // 当たり判定カプセルの半径
    const float hitWidth_;

    // 当たり判定カプセルの高さ
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
    /// 角度の差を取得
    /// </summary>
    /// <param name="target">目標の角度</param>
    /// <returns>角度の差</returns>
    float CalcAngleDiff(float target)const;

    /// <summary>
    /// 移動停止時間の更新
    /// </summary>
    void UpdateMoveStopTime();

    int shadowHandle_;

    float jumpPower_ = 0.0f;

    UPDATE_TYPE nowUpdateType_ = UPDATE_TYPE::Idle;

    int nowMoveStopTime_ = 0;
    int moveStopTime_ = 0;

    int jumpSE_ = -1;
};