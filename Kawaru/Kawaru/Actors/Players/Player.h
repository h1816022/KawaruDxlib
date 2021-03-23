#pragma once
#include <string>
#include <vector>
#include "../Character.h"

class Camera;
class Ghost;

/// <summary>
/// 操作キャラクター
/// </summary>
class Player :
    public Character
{
public:
    Player(Scene& scene, Camera& camera, Ghost& ghost, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Player();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw()override final;

    /// <summary>
    /// プレイヤーが見えるか判定用の、目標座標リストのオフセットを取得
    /// </summary>
    /// <returns>目標座標リストのオフセット</returns>
    const std::vector<VECTOR>& GetLineTraceSamplingOffsets()const;

    /// <summary>
    /// 死亡処理
    /// </summary>
    /// <returns>true : 死亡した false : 既に死亡しているか、倒せない状況にある</returns>
    bool Destroy()override final;

    /// <summary>
    /// 1ゲーム終わった時の処理
    /// </summary>
    void EndGame();

    /// <summary>
    /// おばけを呼べる回数を増やす
    /// </summary>
    void AddCallNum();

    /// <summary>
    /// おばけを呼べる回数を取得
    /// </summary>
    /// <returns></returns>
    int GetCallNum()const;

private:
    /// <summary>
    /// 待機状態の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void IdleUpdate(const Input& input)override final;

    /// <summary>
    /// 移動状態の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void RunUpdate(const Input& input)override final;

    /// <summary>
    /// ジャンプ状態の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void JumpUpdate(const Input& input)override final;

    /// <summary>
    /// 死亡状態の更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void DestroyUpdate(const Input& input)override final;

    /// <summary>
    /// 長さ1.0fの移動ベクトルを計算
    /// </summary>
    /// <param name="upMoveVec">前方向ベクトル</param>
    /// <param name="leftMoveVec">左方向ベクトル</param>
    void CalcUnitMoveVector(VECTOR& upMoveVec, VECTOR& leftMoveVec);

    /// <summary>
    /// 移動ベクトルを計算し代入
    /// </summary>
    /// <param name="moveVec">移動ベクトル</param>
    /// <param name="upMoveVec">前方向ベクトル</param>
    /// <param name="leftMoveVec">左方向ベクトル</param>
    /// <param name="input">入力情報</param>
    /// <returns>true : 移動した</returns>
    bool CalcMoveVector(VECTOR& moveVec, const VECTOR& upMoveVec, const VECTOR& leftMoveVec, const Input& input);

    /// <summary>
    /// 移動用データの更新
    /// </summary>
    /// <param name="input">入力情報</param>
    void UpdateMove(const Input& input);

    /// <summary>
    /// おばけを呼ぶ
    /// </summary>
    /// <returns>true : 呼べた false : 呼べなかったか、既に呼んでいて向かっている</returns>
    bool CallGhost();

    Camera& camera_;
    Ghost& ghost_;

    // トーレスを行う際に終端とするサンプリング座標の、自身の座標からのオフセット値
    std::vector<VECTOR> lineTraceSamplingOffsets_;

    int fingerSnapSE_ = -1;

    int callNum_;
};