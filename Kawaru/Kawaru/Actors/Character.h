#pragma once
#include <memory>
#include "../Geometry.h"
#include "Actor.h"

class Input;
class Stage;
class AnimationComponent;

struct HitCheckPolyData;

enum class UPDATE_TYPE
{
    Idle,
    Run,
    Jump,
    Destroy
};

class Character :
    public Actor
{
public:
    Character(Scene& scene, const Stage& stage, const float hitWidth, const float hitHeight,const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);

    Character(Scene& scene, const wchar_t* modelFilePath, const wchar_t* motionFilePath, const Stage& stage, const float hitWidth, const float hitHeight, 
        const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f);
    virtual ~Character();

    virtual void Update(const Input& input) = 0;
    virtual void Draw() = 0;

    void ChangeUpadater(UPDATE_TYPE type);

    Capsule3D GetCollisionCapsule(const VECTOR& pos)override;

    virtual void Destroy()override;

protected:
    void UpdatePos(const VECTOR& moveVector);

    void GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData);

    bool CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos);

    void CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos);

    void Jump();


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
    virtual void DestroyUpdate(const Input& input);

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

    std::unique_ptr<AnimationComponent> animationComponent_;

private:
    float CalcAngleDiff(float target)const;

    void UpdateMoveStopTime();

    int shadowHandle_;

    float jumpPower_ = 0.0f;

    UPDATE_TYPE nowUpdateType_ = UPDATE_TYPE::Idle;

    int nowMoveStopTime_ = 0;
    int moveStopTime_ = 0;
};