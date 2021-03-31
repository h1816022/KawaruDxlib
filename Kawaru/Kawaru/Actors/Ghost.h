#pragma once
#include <memory>
#include "Character.h"

class Input;
class Stage;
class NavMeshMoveComponent;
class Camera;

/// <summary>
/// カメラを操作するゴースト
/// </summary>
class Ghost :
    public Character
{
public:
    Ghost(Scene& scene, Camera& camera, const Stage& stage, const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    ~Ghost();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    void Update(const Input& input)override final;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    /// <summary>
    /// プレイヤーのもとに呼ぶ
    /// </summary>
    /// <returns>true : 呼べた false : 呼べないか、既に呼ばれて向かっている</returns>
    bool Call();

    /// <summary>
    /// ゲームクリア時の処理
    /// </summary>
    void EndGame();

    void InitPos();

private:
    /// <summary>
    /// プレイヤーのもとに向かう
    /// </summary>
    void ApproachPlayer();

    /// <summary>
    /// 指定座標へ向かう
    /// </summary>
    /// <param name="target">目標の座標</param>
    void ApproachTarget(const VECTOR& target);

    /// <summary>
    /// 指定座標を映すのに適度な距離をとった座標を取得
    /// </summary>
    /// <param name="target">映したい座標</param>
    /// <returns></returns>
    VECTOR CalcApproachPos(const VECTOR& target);

    /// <summary>
    /// 移動終了時の処理
    /// </summary>
    void EndMove();

    std::unique_ptr<NavMeshMoveComponent> navMeshMoveComponent_;

    float floatingOffset_;

    Camera& camera_;

    bool followingPlayer_ = false;

    int floatingCount_ = 0;

    float moveSpeed_;

    VECTOR oldMoveDirection_;

    bool gameEnd_ = false;

    VECTOR goalPos_;

    int moveSE_ = -1;
};