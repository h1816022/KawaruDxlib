#include <DxLib.h>
#include <array>
#include "Character.h"
#include "Stage.h"

namespace
{
	// 影の大きさ
	constexpr float SHADOW_SIZE = 200.0f;

	// 影の落ちる高さ
	constexpr float SHADOW_HEIGHT = 700.0f;

	// 処理するコリジョンポリゴンの最大数
	constexpr int MAX_HIT_COLLISION = 2048;

	// 周囲のポリゴン検出に使用する球のサイズ
	constexpr float COLLISION_SPHERE_SIZE = 2048.0f;

	// 壁押し出し処理の最大試行回数
	constexpr int HIT_TRY_NUM = 16;

	// 押し出し処理でスライドさせる距離
	constexpr float HIT_SLIDE_LENGTH = 5.0f;

	// アニメーションの再生速度
	constexpr float ANIM_PLAY_SPEED = 400.0f;

	// アニメーションのブレンド速度
	constexpr float ANIM_BLEND_SPEED = 0.1f;

	// アニメーションアタッチ番号の、アタッチなしのときの値
	constexpr int ANIM_ATTACH_NUM_INVALID = -1;

	// ジャンプ力
	constexpr float JUMP_POWER = 100.0f;

	// 足を踏み外したときのジャンプ力
	constexpr float FALL_UP_POWER = 20.0f;

	// 重力
	constexpr float GRAVITY = 6.0f;

	// 回転速度
	constexpr float ROT_SPEED = 0.2f;
}

// 衝突判定をするポリゴンに関するデータ
struct HitCheckPolyData
{
	unsigned int wallNum = 0;
	unsigned int floorNum = 0;

	std::array<MV1_COLL_RESULT_POLY*, MAX_HIT_COLLISION> wall;	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	std::array<MV1_COLL_RESULT_POLY*, MAX_HIT_COLLISION> floor;	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
};

Character::Character(const Stage& stage, const float hitWidth, const float hitHeight, const float posX, const float posY, const float posZ):
	Actor(posX, posY, posZ), stage_(stage),
	updater_(&Character::IdleUpdate),
	shadowHandle_(LoadGraph(L"Images/Shadow.tga")), hitWidth_(hitWidth), hitHeight_(hitHeight),
	moveDirection_(VGet(1.0f, 0.0f, 0.0f)), moveVec_(VGet(0.0f, 0.0f, 0.0f))
{
}

Character::Character(const wchar_t* modelFilePath, const wchar_t* motionFilePath, const Stage& stage, const float hitWidth, const float hitHeight, const float posX, const float posY, const float posZ):
	Actor(modelFilePath, motionFilePath, posX, posY, posZ), stage_(stage),
	updater_(&Character::IdleUpdate),
	shadowHandle_(LoadGraph(L"Images/Shadow.tga")), hitWidth_(hitWidth), hitHeight_(hitHeight), 
	moveDirection_(VGet(1.0f, 0.0f, 0.0f)), moveVec_(VGet(0.0f, 0.0f, 0.0f))
{
}

Character::~Character()
{
}

void Character::Update(const Input& input)
{
	MATRIX localMatrix;

	MV1ResetFrameUserLocalMatrix(modelHandle_, 2);

	localMatrix = MV1GetFrameLocalMatrix(modelHandle_, 2);

	localMatrix.m[3][2] = 0.0f;

	MV1SetFrameUserLocalMatrix(modelHandle_, 2, localMatrix);
}

void Character::ChangeUpadater(UPDATE_TYPE type)
{
	if (nowUpdateType_ == type)
	{
		return;
	}

	nowUpdateType_ = type;

	switch (type)
	{
	case UPDATE_TYPE::Idle:
		updater_ = &Character::IdleUpdate;
		break;

	case UPDATE_TYPE::Run:
		updater_ = &Character::RunUpdate;
		break;

	case UPDATE_TYPE::Jump:
		updater_ = &Character::JumpUpdate;
		break;

	default:
		break;
	}
}

void Character::UpdatePos(const VECTOR& moveVector)
{
	// 移動したか
	bool moveFlag;

	// 衝突を検知したか
	bool hitFlag;

	// プレイヤーの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	MV1_COLL_RESULT_POLY_DIM hitDim;

	// ポリゴンの構造体にアクセスするために使用するポインタ
	MV1_COLL_RESULT_POLY* poly;

	VECTOR nowPos;

	// 衝突判定をするポリゴンに関するデータ
	HitCheckPolyData polyData;

	nowPos = VAdd(pos_, moveVector);

	// プレイヤーの周囲にあるステージポリゴンを取得する
	hitDim = stage_.CheckHitSphere(pos_, COLLISION_SPHERE_SIZE + VSize(moveVector));

	// x軸かy軸方向に0.01f以上移動した場合は移動したとする
	moveFlag = (fabs(moveVector.x) > 0.01f || fabs(moveVector.z) > 0.01f);

	GetWallPolyAndFloorPoly(polyData, hitDim);

	// 壁ポリゴンとの当たり判定処理
	if (polyData.wallNum != 0)
	{
		hitFlag = CheckHitWithWall(moveFlag, polyData, moveVector, nowPos);

		// 当たっていたら押し出す
		if (hitFlag)
		{
			Extrude(polyData, nowPos);
		}
	}

	// 床ポリゴンとの当たり判定
	if (polyData.floorNum != 0)
	{
		CheckHitWithFloor(moveFlag, polyData, nowPos);
	}

	// 座標の書き換え
	pos_ = nowPos;

	// モデル座標の更新
	MV1SetPosition(modelHandle_, pos_);

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim);
}

void Character::GetWallPolyAndFloorPoly(HitCheckPolyData& outPolyData, const MV1_COLL_RESULT_POLY_DIM& HitData)
{
	// 検出されたポリゴンの数だけ繰り返し
	for (int i = 0; i < HitData.HitNum; i++)
	{
		// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
		if (HitData.Dim[i].Normal.y < 0.000001f && HitData.Dim[i].Normal.y > -0.000001f)
		{
			// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
			if (HitData.Dim[i].Position[0].y > pos_.y + 1.0f ||
				HitData.Dim[i].Position[1].y > pos_.y + 1.0f ||
				HitData.Dim[i].Position[2].y > pos_.y + 1.0f)
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (outPolyData.wallNum < MAX_HIT_COLLISION)
				{
					// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					outPolyData.wall[outPolyData.wallNum] = &HitData.Dim[i];

					// 壁ポリゴンの数を加算する
					outPolyData.wallNum++;
				}
			}
		}
		else
		{
			// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
			if (outPolyData.floorNum < MAX_HIT_COLLISION)
			{
				// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
				outPolyData.floor[outPolyData.floorNum] = &HitData.Dim[i];

				// 床ポリゴンの数を加算する
				outPolyData.floorNum++;
			}
		}
	}
}

bool Character::CheckHitWithWall(bool moveFlag, const HitCheckPolyData& polyData, const VECTOR& moveVector, VECTOR& nowPos)
{
	// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
	bool isHit = false;

	MV1_COLL_RESULT_POLY* poly;

	// 移動したかどうかで処理を分岐
	if (moveFlag == true)
	{
		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < polyData.wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			poly = polyData.wall[i];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_, poly->Position[0], poly->Position[1], poly->Position[2]) == false)
			{
				continue;
			}

			// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
			isHit = true;

			// 壁に当たったら壁に遮られない移動成分分だけ移動する
			{
				VECTOR slideVec;	// プレイヤーをスライドさせるベクトル

				// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
				slideVec = VCross(moveVector, poly->Normal);

				// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
				// 元の移動成分から壁方向の移動成分を抜いたベクトル
				slideVec = VCross(poly->Normal, slideVec);

				// それを移動前の座標に足したものを新たな座標とする
				nowPos = VAdd(pos_, slideVec);
			}

			int j;

			// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
			for (j = 0; j < polyData.wallNum; j++)
			{
				// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				poly = polyData.wall[j];

				// 当たっていたらループから抜ける
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_, poly->Position[0], poly->Position[1], poly->Position[2]) == true) break;
			}

			// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
			// 壁に当たったフラグを倒した上でループから抜ける
			if (j == polyData.wallNum)
			{
				isHit = false;
				break;
			}
		}
	}
	else
	{
		// 移動していない場合の処理

		// 壁ポリゴンの数だけ繰り返し
		for (int i = 0; i < polyData.wallNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			poly = polyData.wall[i];

			// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_, poly->Position[0], poly->Position[1], poly->Position[2]) == true)
			{
				isHit = true;
				break;
			}
		}
	}

	return isHit;
}

void Character::CheckHitWithFloor(bool moveFlag, const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	HITRESULT_LINE lineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体

	bool isHit = false;

	// ジャンプ中且つ上昇中の場合は処理を分岐
	if (updater_ == &Character::JumpUpdate && jumpPower_ > 0.0f)
	{
		float minY;

		// 天井に頭をぶつける処理を行う

		// 一番低い天井にぶつける為の判定用変数を初期化
		minY = 0.0f;

		// 当たったかどうかのフラグを当たっていないを意味する０にしておく

		// 床ポリゴンの数だけ繰り返し
		for (int i = 0; i < polyData.floorNum; i++)
		{
			// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
			poly = polyData.floor[i];

			// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
			lineRes = HitCheck_Line_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);

			// 接触していなかったら何もしない
			if (!lineRes.HitFlag)
			{
				continue;
			}

			// 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
			if (isHit && minY < lineRes.Position.y)
			{
				continue;
			}

			// ポリゴンに当たったフラグを立てる
			isHit = true;

			// 接触したＹ座標を保存する
			minY = lineRes.Position.y;
		}

		// 接触したポリゴンがあったかどうかで処理を分岐
		if (isHit)
		{
			// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
			nowPos.y = minY - hitHeight_;

			// Ｙ軸方向の速度は反転
			jumpPower_ = -jumpPower_;
		}
	}
	else
	{
		float maxY;

		// 下降中かジャンプ中ではない場合の処理

		// 床ポリゴンに当たったかどうかのフラグを倒しておく
		isHit = false;

		// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
		maxY = 0.0f;

		// 床ポリゴンの数だけ繰り返し
		for (int i = 0; i < polyData.floorNum; i++)
		{
			// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
			poly = polyData.floor[i];

			// ジャンプ中かどうかで処理を分岐
			if (updater_ == &Character::JumpUpdate)
			{
				// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
				lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), VAdd(nowPos, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
			}
			else
			{
				// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
				lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), VAdd(nowPos, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
			}

			// 当たっていなかったら何もしない
			if (!lineRes.HitFlag)
			{
				continue;
			}

			// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
			if (isHit && maxY > lineRes.Position.y)
			{
				continue;
			}

			// ポリゴンに当たったフラグを立てる
			isHit = true;

			// 接触したＹ座標を保存する
			maxY = lineRes.Position.y;
		}

		// 床ポリゴンに当たったかどうかで処理を分岐
		if (isHit)
		{
			// 当たった場合

			// 「接触したポリゴンで一番高いＹ座標」と現在のY座標の高い方を改めてＹ座標とする
			nowPos.y = max(nowPos.y, maxY);

			// Ｙ軸方向の移動速度は０に
			jumpPower_ = 0.0f;

			// もしジャンプ中だった場合は着地状態にする
			if (updater_ == &Character::JumpUpdate)
			{
				// 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
				if (moveFlag)
				{
					// 移動している場合は走り状態に
					ChangeAnim(ANIM_NAME::Walk);
					ChangeUpadater(UPDATE_TYPE::Run);
				}
				else
				{
					// 移動していない場合は立ち止り状態に
					ChangeAnim(ANIM_NAME::Idle);
					ChangeUpadater(UPDATE_TYPE::Idle);
				}

				// 着地時はアニメーションのブレンドは行わない
				animBlendRate_ = 1.0f;
			}
		}
		else
		{
			// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
			if (updater_ != &Character::JumpUpdate)
			{
				// ジャンプ中にする
				ChangeUpadater(UPDATE_TYPE::Jump);

				// ちょっとだけジャンプする
				jumpPower_ = FALL_UP_POWER;

				// アニメーションは落下中のものにする
				ChangeAnim(ANIM_NAME::Fall);
			}
		}
	}
}

void Character::Jump()
{
	ChangeUpadater(UPDATE_TYPE::Jump);

	jumpPower_ = JUMP_POWER;

	moveVec_.y = jumpPower_;

	ChangeAnim(ANIM_NAME::Jump);
}

void Character::Extrude(const HitCheckPolyData& polyData, VECTOR& nowPos)
{
	MV1_COLL_RESULT_POLY* poly;

	// 壁からの押し出し処理を試みる最大数だけ繰り返し
	for (int tryNum = 0; tryNum < HIT_TRY_NUM; tryNum++)
	{
		// 壁ポリゴンの数だけ繰り返し
		int polyIndex;

		for (polyIndex = 0; polyIndex < polyData.wallNum; polyIndex++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			poly = polyData.wall[polyIndex];

			// プレイヤーと当たっているかを判定
			if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_, poly->Position[0], poly->Position[1], poly->Position[2]) == false)
			{
				continue;
			}

			// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
			nowPos = VAdd(nowPos, VScale(VNorm(VSub(nowPos, poly->HitPosition)), HIT_SLIDE_LENGTH));

			// 移動した上で壁ポリゴンと接触しているかどうかを判定
			int wallIndex;

			for (wallIndex = 0; wallIndex < polyData.wallNum; wallIndex++)
			{
				// 当たっていたらループを抜ける
				poly = polyData.wall[wallIndex];
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, hitHeight_, 0.0f)), hitWidth_, poly->Position[0], poly->Position[1], poly->Position[2]) == true)
				{
					break;
				}
			}

			// 全てのポリゴンと当たっていなかったらここでループ終了
			if (wallIndex == polyData.wallNum)
			{
				break;
			}
		}

		// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
		if (polyIndex != polyData.wallNum)
		{
			break;
		}
	}
}

void Character::DrawShadow()
{
	MV1_COLL_RESULT_POLY_DIM hitResDim;
	MV1_COLL_RESULT_POLY* hitRes;
	VERTEX3D vertex[3];
	VECTOR slideVec;

	// ライティングを無効にする
	SetUseLighting(false);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(true);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// プレイヤーの直下に存在する地面のポリゴンを取得
	hitResDim = stage_.CheckHitCapsule(pos_, VAdd(pos_, VGet(0.0f, -SHADOW_HEIGHT, 0.0f)), SHADOW_SIZE);

	// 頂点データで変化が無い部分をセット
	vertex[0].dif = GetColorU8(255, 255, 255, 255);
	vertex[0].spc = GetColorU8(0, 0, 0, 0);
	vertex[0].su = 0.0f;
	vertex[0].sv = 0.0f;
	vertex[1] = vertex[0];
	vertex[2] = vertex[0];

	// 球の直下に存在するポリゴンの数だけ繰り返し
	hitRes = hitResDim.Dim;
	for (int i = 0; i < hitResDim.HitNum; i++, hitRes++)
	{
		// ポリゴンの座標は地面ポリゴンの座標
		vertex[0].pos = hitRes->Position[0];
		vertex[1].pos = hitRes->Position[1];
		vertex[2].pos = hitRes->Position[2];

		// ちょっと持ち上げて重ならないようにする
		slideVec = VScale(hitRes->Normal, 0.5f);
		vertex[0].pos = VAdd(vertex[0].pos, slideVec);
		vertex[1].pos = VAdd(vertex[1].pos, slideVec);
		vertex[2].pos = VAdd(vertex[2].pos, slideVec);

		// ポリゴンの不透明度を設定する
		vertex[0].dif.a = 0;
		vertex[1].dif.a = 0;
		vertex[2].dif.a = 0;
		if (hitRes->Position[0].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[0].dif.a = 128 * (1.0f - fabs(hitRes->Position[0].y - pos_.y) / SHADOW_HEIGHT);
		}

		if (hitRes->Position[1].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[1].dif.a = 128 * (1.0f - fabs(hitRes->Position[1].y - pos_.y) / SHADOW_HEIGHT);
		}

		if (hitRes->Position[2].y > pos_.y - SHADOW_HEIGHT)
		{
			vertex[2].dif.a = 128 * (1.0f - fabs(hitRes->Position[2].y - pos_.y) / SHADOW_HEIGHT);
		}

		// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
		vertex[0].u = (hitRes->Position[0].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[0].v = (hitRes->Position[0].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].u = (hitRes->Position[1].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[1].v = (hitRes->Position[1].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].u = (hitRes->Position[2].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		vertex[2].v = (hitRes->Position[2].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;

		// 影ポリゴンを描画
		DrawPolygon3D(vertex, 1, shadowHandle_, true);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hitResDim);

	// ライティングを有効にする
	SetUseLighting(true);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(false);
}

void Character::IdleUpdate(const Input& input)
{
}

void Character::RunUpdate(const Input& input)
{
}

void Character::JumpUpdate(const Input& input)
{
	jumpPower_ -= GRAVITY;

	if (jumpPower_ < 0.0f && CheckNowAnim(animAttachNum1_, ANIM_NAME::Jump))
	{
		ChangeAnim(ANIM_NAME::Fall);
	}

	moveVec_.y = jumpPower_;
}

void Character::ChangeAnim(ANIM_NAME playAnim)
{
	// 再生中のモーション２が有効だったらデタッチする
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		DetachAnim(animAttachNum2_);
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	animAttachNum2_ = animAttachNum1_;
	animPlayCount2_ = animPlayCount1_;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	animAttachNum1_ = AttachAnim(playAnim);
	animPlayCount1_ = 0.0f;

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	animBlendRate_ = (animAttachNum2_ == ANIM_ATTACH_NUM_INVALID) ? 1.0f : 0.0f;
}

void Character::UpdateAnim()
{
	float totalTime;

	UpdateAnimBlendRate();

	// 再生しているアニメーション１の処理
	if (animAttachNum1_ != ANIM_ATTACH_NUM_INVALID)
	{
		// アニメーションの総時間を取得
		totalTime = GetAnimTotalTime(animAttachNum1_);

		// 再生時間を進める
		animPlayCount1_ += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (animPlayCount1_ >= totalTime)
		{
			animPlayCount1_ = fmod(animPlayCount1_, totalTime);
		}

		// 変更した再生時間をモデルに反映させる
		SetAnimTime(animAttachNum1_, animPlayCount1_);

		// アニメーション１のモデルに対する反映率をセット
		SetAnimBlendRate(animAttachNum1_, animBlendRate_);
	}

	// 再生しているアニメーション２の処理
	if (animAttachNum2_ != ANIM_ATTACH_NUM_INVALID)
	{
		// アニメーションの総時間を取得
		totalTime = GetAnimTotalTime(animAttachNum2_);

		// 再生時間を進める
		animPlayCount2_ += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (animPlayCount2_ > totalTime)
		{
			animPlayCount2_ = fmod(animPlayCount2_, totalTime);
		}

		// 変更した再生時間をモデルに反映させる
		SetAnimTime(animAttachNum2_, animPlayCount2_);

		// アニメーション２のモデルに対する反映率をセット
		SetAnimBlendRate(animAttachNum2_, 1.0f - animBlendRate_);
	}
}

void Character::UpdateAngle()
{
	float targetAngle;			// 目標角度
	float diffAngle;			// 目標角度と現在の角度との差

	// 目標の方向ベクトルから角度値を算出する
	targetAngle = atan2(moveDirection_.x, moveDirection_.z);

	// 目標の角度と現在の角度との差を割り出す
	diffAngle = CalcAngleDiff(targetAngle);

	// 角度の差が０に近づける
	if (diffAngle > 0.0f)
	{
		// 差がプラスの場合は引く
		diffAngle -= ROT_SPEED;
		if (diffAngle < 0.0f)
		{
			diffAngle = 0.0f;
		}
	}
	else
	{
		// 差がマイナスの場合は足す
		diffAngle += ROT_SPEED;
		if (diffAngle > 0.0f)
		{
			diffAngle = 0.0f;
		}
	}

	// モデルの角度を更新
	angle_ = targetAngle - diffAngle;
	MV1SetRotationXYZ(modelHandle_, VGet(0.0f, angle_ + DX_PI_F, 0.0f));
}

void Character::UpdateAnimBlendRate()
{
	if (animBlendRate_ >= 1.0f)
	{
		return;
	}

	animBlendRate_ = min(animBlendRate_ + ANIM_BLEND_SPEED, 1.0f);
}

bool Character::CheckNowAnim(int animAttachNum, ANIM_NAME target) const
{
	return MV1GetAttachAnim(modelHandle_, animAttachNum) == static_cast<int>(target);
}

int Character::AttachAnim(ANIM_NAME animName)
{
	return MV1AttachAnim(modelHandle_, static_cast<int>(animName));
}

void Character::DetachAnim(int animAttachNum)
{
	MV1DetachAnim(modelHandle_, animAttachNum);
}

float Character::GetAnimTotalTime(int animattachNum)
{
	return MV1GetAttachAnimTotalTime(modelHandle_, animattachNum);
}

void Character::SetAnimTime(int animAttachNum, int count)
{
	MV1SetAttachAnimTime(modelHandle_, animAttachNum, count);
}

void Character::SetAnimBlendRate(int animAttachNum, float rate)
{
	MV1SetAttachAnimBlendRate(modelHandle_, animAttachNum, rate);
}

float Character::CalcAngleDiff(float target)const
{
	float ret = target - angle_;

	if (ret < -DX_PI_F)
	{
		ret += DX_TWO_PI_F;
	}
	else if (ret > DX_PI_F)
	{
		ret -= DX_TWO_PI_F;
	}

	return ret;
}