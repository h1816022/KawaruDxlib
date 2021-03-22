#pragma once
#include <stdint.h>
#include <memory>
#include <string>
#include "../Geometry.h"

class SceneController;
class Input;
class Actor;

// フェードの種類
enum class FADE_MODE
{
	Non,	// フェードなし
	In,		// だんだん晴れる
	Out		// だんだん真っ暗に
};

namespace
{
	// 基本のフェードにかける時間
	constexpr uint32_t DEFAULT_FADE_INTERVAL = 60;

	// 画面枠の大きさ
	constexpr int LETTERBOX_MASK_LENGTH = 50;

	// シーンの表示を重ねるときのマスクの透明度
	constexpr int OVERLAP_TRANSPARENCY = 120;

	// メニューの透明度
	constexpr int MENU_TRANSPARENCY = 200;
}

/// <summary>
/// シーン基底クラス
/// </summary>
class Scene
{
	friend SceneController;

public:
	virtual ~Scene();

	/// <summary>
	///  更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	virtual void Update(const Input& input) {};
	
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() {};

	Scene(const Scene& scene) = default;

	/// <summary>
	/// フェードイン終了時の処理
	/// </summary>
	virtual void EndFadeIn() {};

	/// <summary>
	///  フェードアウト終了時の処理
	/// </summary>
	virtual void EndFadeOut() {};

	/// <summary>
	/// アクターの追加登録
	/// </summary>
	/// <param name="actor">登録するアクター</param>
	void AddActors(std::shared_ptr<Actor> actor);

	/// <summary>
	/// 登録されているアクターから、指定タグに該当するものを全て取得
	/// </summary>
	/// <param name="tag">取得したいアクターのタグ</param>
	/// <returns>指定タグのアクターのリスト</returns>
	std::vector<std::shared_ptr<Actor>> GetActors(const std::wstring& tag);

protected:
	Scene(SceneController& controller);

	/// <summary>
	/// フェード処理の更新
	/// </summary>
	/// <param name="input">入力情報</param>
	void UpdateFade(const Input& input);

	/// <summary>
	/// フェードを開始する
	/// </summary>
	/// <param name="mode">イン / アウトを指定</param>
	/// <param name="time">フェードに掛ける時間</param>
	void StartFade(const FADE_MODE& mode, int time = DEFAULT_FADE_INTERVAL);

	/// <summary>
	/// 様々な描画の後の画面効果(フェードなど)を描画
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 影付きの文字列を描画
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="text">描画する文字列</param>
	/// <param name="color">文字色</param>
	void DrawDropShadowString(const Position2& pos, const wchar_t* text, unsigned int color = 0xffffff);

	/// <summary>
	/// 登録アクター達の更新処理
	/// </summary>
	/// <param name="input">入力情報</param>
	void UpdateActors(const Input& input);

	/// <summary>
	/// 登録アクター達の描画処理
	/// </summary>
	void DrawActors();

	SceneController& controller_;

private:
	/// <summary>
	/// フェード処理
	/// </summary>
	/// <param name="rate">濃度</param>
	void Fade(float rate);

	FADE_MODE fadeMode_;

	int nowFadeCount_;

	int fadeInterval_;

	float nowFadeRate = 0.0f;

	std::vector<std::shared_ptr<Actor>> actors_;
};