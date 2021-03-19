#pragma once

/// <summary>
/// リザルトに表示する、シーンをまたいで記録、使用するデータ
/// </summary>
struct ResultData
{
	ResultData();

	/// <summary>
	/// データの初期化
	/// </summary>
	void Reset();

	// プレイしていた時間
	unsigned int playTime = 0;

	// ゴーストを呼んだ回数
	unsigned int callCount = 0;

	// クリアしたか
	bool gameClear = false;

	// ゲームオーバーになったか
	bool gameOver = false;
};