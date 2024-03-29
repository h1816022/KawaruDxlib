#pragma once
#include <memory>
#include "../Actor.h"

class Scene;

/// <summary>
/// 指定タグのアクターとの当たり判定を取るアクター基底クラス
/// </summary>
class Gimmick :
    public Actor
{
public:
    Gimmick(Scene& scene, const wchar_t* modelFilePath, const wchar_t* targetTag, bool onceHit, float px, float py, float pz);
    virtual ~Gimmick();

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="input">入力情報</param>
    virtual void Update(const Input& input)override;

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw() = 0;

protected:
    /// <summary>
    /// 当たり判定を検知した時の処理
    /// </summary>
    /// <param name="hitActor">当たったアクター</param>
    /// <returns>true : 処理を完遂した</returns>
    virtual bool Hit(std::shared_ptr<Actor> hitActor) = 0;

    const std::wstring targetTag_;

    Scene& scene_;

    bool collisionEnabled_ = true;

private:
    /// <summary>
    /// 当たり判定をする
    /// </summary>
    /// <param name="actor">調べるアクター</param>
    /// <returns>true : ヒットした</returns>
    bool CheckHit(std::shared_ptr<Actor> actor);

    const bool onceHit_;

    bool hit_ = false;
};

