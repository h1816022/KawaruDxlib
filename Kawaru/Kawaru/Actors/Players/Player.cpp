#include <Dxlib.h>
#include "Player.h"
#include "../Camera.h"
#include "../Stage.h"
#include "../../Systems/Input.h"

namespace
{
	// 移動速度
	constexpr float MOVE_SPEED = 30.0f;
	
	// ジャンプ力
	constexpr float JUMP_POWER = 100.0f;
	
	// 足を踏み外したときのジャンプ力
	constexpr float FALL_UP_POWER = 20.0f;
	
	// 重力
	constexpr float GRAVITY = 3.0f;

	// 回転速度
	constexpr float ROT_SPEED = 0.2f;

	// 処理するコリジョンポリゴンの最大数
	constexpr int MAX_HIT_COLLISION = 2048;

	// 壁押し出し処理の最大試行回数
	constexpr int HIT_TRY_NUM = 16;

	// 周囲のポリゴン検出に使用する球の初期サイズ
	constexpr float COLLISION_SPHERE_SIZE = 2048.0f;
	
	// 当たり判定カプセルの半径
	constexpr float HIT_WIDTH = 200.0f;

	// 当たり判定カプセルの高さ
	constexpr float HIT_HEIGHT = 700.0f;

	// 押し出し処理でスライドさせる距離
	constexpr float HIT_SLIDE_LENGTH = 5.0f;

	// アニメーションの再生速度
	constexpr float ANIM_PLAY_SPEED = 250.0f;
	
	// アニメーションのブレンド速度
	constexpr float ANIM_BLEND_SPEED = 0.1f;

	// 影の大きさ
	constexpr float SHADOW_SIZE = 200.0f;
	
	// 影の落ちる高さ
	constexpr float SHADOW_HEIGHT = 700.0f;
}

Player::Player(const Camera& camera, const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(L"Models/DxChara.x", posX, posY, posZ), camera_(camera), stage_(stage), updater_(&Player::IdleUpdate)
{
	moveDirection_ = VGet(1.0f, 0.0f, 0.0f);
	shadowHandle_ = LoadGraph(L"Images/Shadow.tga");
}

Player::~Player()
{
}

void Player::Update(const Input& input)
{
	(this->*updater_)();

	VECTOR upMoveVec;
	VECTOR leftMoveVec;
	VECTOR moveVec;
	int moveFlag;

	MATRIX localMatrix;

	// ユーザー行列を解除する
	MV1ResetFrameUserLocalMatrix(modelHandle_, 2);

	// 現在のルートフレームの行列を取得する
	localMatrix = MV1GetFrameLocalMatrix(modelHandle_, 2);

	// Ｚ軸方向の平行移動成分を無効にする
	localMatrix.m[3][2] = 0.0f;

	// ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
	MV1SetFrameUserLocalMatrix(modelHandle_, 2, localMatrix);

	// プレイヤーの移動方向のベクトルを算出
	{
		// 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
		upMoveVec = VSub(camera_.GetTargetPos(), camera_.GetPos());
		upMoveVec.y = 0.0f;

		// 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
		leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

		// 二つのベクトルを正規化( ベクトルの長さを１．０にすること )
		upMoveVec = VNorm(upMoveVec);
		leftMoveVec = VNorm(leftMoveVec);
	}

	// このフレームでの移動ベクトルを初期化
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	// 移動したかどうかのフラグを初期状態では「移動していない」を表す０にする
	moveFlag = 0;

	// 移動処理
	// 方向ボタン「←」が入力されたらカメラの見ている方向から見て左方向に移動する
	if (input.IsPressed("Left"))
	{
		// 移動ベクトルに「←」が入力された時の移動ベクトルを加算する
		moveVec = VAdd(moveVec, leftMoveVec);

		// 移動したかどうかのフラグを「移動した」にする
		moveFlag = 1;
	}
	else
		// 方向ボタン「→」が入力されたらカメラの見ている方向から見て右方向に移動する
		if (input.IsPressed("Right"))
		{
			// 移動ベクトルに「←」が入力された時の移動ベクトルを反転したものを加算する
			moveVec = VAdd(moveVec, VScale(leftMoveVec, -1.0f));

			// 移動したかどうかのフラグを「移動した」にする
			moveFlag = 1;
		}

	// 方向ボタン「↑」が入力されたらカメラの見ている方向に移動する
	if (input.IsPressed("Up"))
	{
		// 移動ベクトルに「↑」が入力された時の移動ベクトルを加算する
		moveVec = VAdd(moveVec, upMoveVec);

		// 移動したかどうかのフラグを「移動した」にする
		moveFlag = 1;
	}
	else
	// 方向ボタン「↓」が入力されたらカメラの方向に移動する
	if (input.IsPressed("Down"))
	{
		// 移動ベクトルに「↑」が入力された時の移動ベクトルを反転したものを加算する
		moveVec = VAdd(moveVec, VScale(upMoveVec, -1.0f));

		// 移動したかどうかのフラグを「移動した」にする
		moveFlag = 1;
	}

	// プレイヤーの状態が「ジャンプ」ではなく、且つボタン１が押されていたらジャンプする
	if (state_ != 2 && (input.IsTriggered("Jump")))
	{
		// 状態を「ジャンプ」にする
		state_ = 2;

		// Ｙ軸方向の速度をセット
		jumpPower_ = JUMP_POWER;

		// ジャンプアニメーションの再生
		ChangeAnim(PLAYER_ANIM_NAME::JUMP);
	}

	// 移動ボタンが押されたかどうかで処理を分岐
	if (moveFlag)
	{
		// 移動ベクトルを正規化したものをプレイヤーが向くべき方向として保存
		moveDirection_ = VNorm(moveVec);

		// プレイヤーが向くべき方向ベクトルをプレイヤーのスピード倍したものを移動ベクトルとする
		moveVec = VScale(moveDirection_, MOVE_SPEED);

		// もし今まで「立ち止まり」状態だったら
		if (state_ == 0)
		{
			ChangeAnim(PLAYER_ANIM_NAME::RUN);

			// 状態を「走り」にする
			state_ = 1;
		}
	}
	else
	{
		// このフレームで移動していなくて、且つ状態が「走り」だったら
		if (state_ == 1)
		{
			// 立ち止りアニメーションを再生する
			ChangeAnim(PLAYER_ANIM_NAME::IDLE);

			// 状態を「立ち止り」にする
			state_ = 0;
		}
	}

	// 状態が「ジャンプ」の場合は
	if (state_ == 2)
	{
		// Ｙ軸方向の速度を重力分減算する
		jumpPower_ -= GRAVITY;

		// もし落下していて且つ再生されているアニメーションが上昇中用のものだった場合は
		if (jumpPower_ < 0.0f && MV1GetAttachAnim(modelHandle_, playAnim1_) == 2)
		{
			// 落下中ようのアニメーションを再生する
			ChangeAnim(PLAYER_ANIM_NAME::FALL);
		}

		// 移動ベクトルのＹ成分をＹ軸方向の速度にする
		moveVec.y = jumpPower_;
	}

	// プレイヤーの移動方向にモデルの方向を近づける
	UpdateAngle();

	// 移動ベクトルを元にコリジョンを考慮しつつプレイヤーを移動
	Move(moveVec);

	// アニメーション処理
	UpdateAnim();
}

void Player::Draw()
{
	MV1DrawModel(modelHandle_);

	DrawShadow();
}

void Player::IdleUpdate()
{
}

void Player::RunUpdate()
{
}

void Player::JumpUpdate()
{
}

void Player::Move(const VECTOR& moveVector)
{
	int i, j, k;						// 汎用カウンタ変数
	int moveFlag;						// 水平方向に移動したかどうかのフラグ( ０:移動していない  １:移動した )
	int hitFlag;						// ポリゴンに当たったかどうかを記憶しておくのに使う変数( ０:当たっていない  １:当たった )
	MV1_COLL_RESULT_POLY_DIM hitDim;			// プレイヤーの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	int kabeNum;						// 壁ポリゴンと判断されたポリゴンの数
	int yukaNum;						// 床ポリゴンと判断されたポリゴンの数
	MV1_COLL_RESULT_POLY* kabe[MAX_HIT_COLLISION];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* yuka[MAX_HIT_COLLISION];	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* poly;				// ポリゴンの構造体にアクセスするために使用するポインタ( 使わなくても済ませられますがプログラムが長くなるので・・・ )
	HITRESULT_LINE lineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
	VECTOR oldPos;						// 移動前の座標	
	VECTOR nowPos;						// 移動後の座標

	// 移動前の座標を保存
	oldPos = pos_;

	// 移動後の座標を算出
	nowPos = VAdd(pos_, moveVector);

	// プレイヤーの周囲にあるステージポリゴンを取得する
	// ( 検出する範囲は移動距離も考慮する )
	hitDim = stage_.CollCheckSphere(pos_, COLLISION_SPHERE_SIZE + VSize(moveVector));

	// x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
	if (fabs(moveVector.x) > 0.01f || fabs(moveVector.z) > 0.01f)
	{
		moveFlag = 1;
	}
	else
	{
		moveFlag = 0;
	}

	// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
	{
		// 壁ポリゴンと床ポリゴンの数を初期化する
		kabeNum = 0;
		yukaNum = 0;

		// 検出されたポリゴンの数だけ繰り返し
		for (i = 0; i < hitDim.HitNum; i++)
		{
			// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
			if (hitDim.Dim[i].Normal.y < 0.000001f && hitDim.Dim[i].Normal.y > -0.000001f)
			{
				// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
				if (hitDim.Dim[i].Position[0].y > pos_.y + 1.0f ||
					hitDim.Dim[i].Position[1].y > pos_.y + 1.0f ||
					hitDim.Dim[i].Position[2].y > pos_.y + 1.0f)
				{
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (kabeNum < MAX_HIT_COLLISION)
					{
						// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
						kabe[kabeNum] = &hitDim.Dim[i];

						// 壁ポリゴンの数を加算する
						kabeNum++;
					}
				}
			}
			else
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (yukaNum < MAX_HIT_COLLISION)
				{
					// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
					yuka[yukaNum] = &hitDim.Dim[i];

					// 床ポリゴンの数を加算する
					yukaNum++;
				}
			}
		}
	}

	// 壁ポリゴンとの当たり判定処理
	if (kabeNum != 0)
	{
		// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
		hitFlag = 0;

		// 移動したかどうかで処理を分岐
		if (moveFlag == 1)
		{
			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < kabeNum; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				poly = kabe[i];

				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

				// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				hitFlag = 1;

				// 壁に当たったら壁に遮られない移動成分分だけ移動する
				{
					VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(moveVector, poly->Normal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
					// 元の移動成分から壁方向の移動成分を抜いたベクトル
					SlideVec = VCross(poly->Normal, SlideVec);

					// それを移動前の座標に足したものを新たな座標とする
					nowPos = VAdd(oldPos, SlideVec);
				}

				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < kabeNum; j++)
				{
					// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe[j];

					// 当たっていたらループから抜ける
					if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
				}

				// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
				// 壁に当たったフラグを倒した上でループから抜ける
				if (j == kabeNum)
				{
					hitFlag = 0;
					break;
				}
			}
		}
		else
		{
			// 移動していない場合の処理

			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < kabeNum; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				poly = kabe[i];

				// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
				if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE)
				{
					hitFlag = 1;
					break;
				}
			}
		}

		// 壁に当たっていたら壁から押し出す処理を行う
		if (hitFlag == 1)
		{
			// 壁からの押し出し処理を試みる最大数だけ繰り返し
			for (k = 0; k < HIT_TRY_NUM; k++)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < kabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					poly = kabe[i];

					// プレイヤーと当たっているかを判定
					if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == FALSE) continue;

					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
					nowPos = VAdd(nowPos, VScale(poly->Normal, HIT_SLIDE_LENGTH));

					// 移動した上で壁ポリゴンと接触しているかどうかを判定
					for (j = 0; j < kabeNum; j++)
					{
						// 当たっていたらループを抜ける
						poly = kabe[j];
						if (HitCheck_Capsule_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), HIT_WIDTH, poly->Position[0], poly->Position[1], poly->Position[2]) == TRUE) break;
					}

					// 全てのポリゴンと当たっていなかったらここでループ終了
					if (j == kabeNum) break;
				}

				// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
				if (i != kabeNum) break;
			}
		}
	}

	// 床ポリゴンとの当たり判定
	if (yukaNum != 0)
	{
		// ジャンプ中且つ上昇中の場合は処理を分岐
		if (state_ == 2 && jumpPower_ > 0.0f)
		{
			float MinY;

			// 天井に頭をぶつける処理を行う

			// 一番低い天井にぶつける為の判定用変数を初期化
			MinY = 0.0f;

			// 当たったかどうかのフラグを当たっていないを意味する０にしておく
			hitFlag = 0;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < yukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				poly = yuka[i];

				// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
				lineRes = HitCheck_Line_Triangle(nowPos, VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);

				// 接触していなかったら何もしない
				if (lineRes.HitFlag == FALSE) continue;

				// 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
				if (hitFlag == 1 && MinY < lineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				hitFlag = 1;

				// 接触したＹ座標を保存する
				MinY = lineRes.Position.y;
			}

			// 接触したポリゴンがあったかどうかで処理を分岐
			if (hitFlag == 1)
			{
				// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
				nowPos.y = MinY - HIT_HEIGHT;

				// Ｙ軸方向の速度は反転
				jumpPower_ = -jumpPower_;
			}
		}
		else
		{
			float MaxY;

			// 下降中かジャンプ中ではない場合の処理

			// 床ポリゴンに当たったかどうかのフラグを倒しておく
			hitFlag = 0;

			// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
			MaxY = 0.0f;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < yukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				poly = yuka[i];

				// ジャンプ中かどうかで処理を分岐
				if (state_ == 2)
				{
					// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
					lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), VAdd(nowPos, VGet(0.0f, -1.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}
				else
				{
					// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
					lineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, HIT_HEIGHT, 0.0f)), VAdd(nowPos, VGet(0.0f, -40.0f, 0.0f)), poly->Position[0], poly->Position[1], poly->Position[2]);
				}

				// 当たっていなかったら何もしない
				if (lineRes.HitFlag == FALSE) continue;

				// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (hitFlag == 1 && MaxY > lineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				hitFlag = 1;

				// 接触したＹ座標を保存する
				MaxY = lineRes.Position.y;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (hitFlag == 1)
			{
				// 当たった場合

				// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
				nowPos.y = MaxY;

				// Ｙ軸方向の移動速度は０に
				jumpPower_ = 0.0f;

				// もしジャンプ中だった場合は着地状態にする
				if (state_ == 2)
				{
					// 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
					if (moveFlag)
					{
						// 移動している場合は走り状態に
						ChangeAnim(PLAYER_ANIM_NAME::RUN);
						state_ = 1;
					}
					else
					{
						// 移動していない場合は立ち止り状態に
						ChangeAnim(PLAYER_ANIM_NAME::IDLE);
						state_ = 0;
					}

					// 着地時はアニメーションのブレンドは行わない
					animBlendRate_ = 1.0f;
				}
			}
			else
			{
				// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
				if (state_ != 2)
				{
					// ジャンプ中にする
					state_ = 2;

					// ちょっとだけジャンプする
					jumpPower_ = FALL_UP_POWER;

					// アニメーションは落下中のものにする
					ChangeAnim(PLAYER_ANIM_NAME::FALL);
				}
			}
		}
	}

	// 新しい座標を保存する
	pos_ = nowPos;

	// プレイヤーのモデルの座標を更新する
	MV1SetPosition(modelHandle_, pos_);

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim);

}

void Player::UpdateAngle()
{
	float targetAngle;			// 目標角度
	float diffAngle;			// 目標角度と現在の角度との差

	// 目標の方向ベクトルから角度値を算出する
	targetAngle = atan2(moveDirection_.x, moveDirection_.z);

	// 目標の角度と現在の角度との差を割り出す
	{
		// 最初は単純に引き算
		diffAngle = targetAngle - angle_;

		// ある方向からある方向の差が１８０度以上になることは無いので
		// 差の値が１８０度以上になっていたら修正する
		if (diffAngle < -DX_PI_F)
		{
			diffAngle += DX_TWO_PI_F;
		}
		else
			if (diffAngle > DX_PI_F)
			{
				diffAngle -= DX_TWO_PI_F;
			}
	}

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

void Player::ChangeAnim(PLAYER_ANIM_NAME playAnim)
{
	// 再生中のモーション２が有効だったらデタッチする
	if (playAnim2_ != -1)
	{
		MV1DetachAnim(modelHandle_, playAnim2_);
		playAnim2_ = -1;
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	playAnim2_ = playAnim1_;
	animPlayCount2_ = animPlayCount1_;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	playAnim1_ = MV1AttachAnim(modelHandle_, static_cast<int>(playAnim));
	animPlayCount1_ = 0.0f;

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	animBlendRate_ = playAnim2_ == -1 ? 1.0f : 0.0f;
}

void Player::UpdateAnim()
{
	float totalTime;

	UpdateAnimBlendRate();

	// 再生しているアニメーション１の処理
	if (playAnim1_ != -1)
	{
		// アニメーションの総時間を取得
		totalTime = MV1GetAttachAnimTotalTime(modelHandle_, playAnim1_);

		// 再生時間を進める
		animPlayCount1_ += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (animPlayCount1_ >= totalTime)
		{
			animPlayCount1_ = fmod(animPlayCount1_, totalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(modelHandle_, playAnim1_, animPlayCount1_);

		// アニメーション１のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(modelHandle_, playAnim1_, animBlendRate_);
	}

	// 再生しているアニメーション２の処理
	if (playAnim2_ != -1)
	{
		// アニメーションの総時間を取得
		totalTime = MV1GetAttachAnimTotalTime(modelHandle_, playAnim2_);

		// 再生時間を進める
		animPlayCount2_ += ANIM_PLAY_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (animPlayCount2_ > totalTime)
		{
			animPlayCount2_ = fmod(animPlayCount2_, totalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(modelHandle_, playAnim2_, animPlayCount2_);

		// アニメーション２のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(modelHandle_, playAnim2_, 1.0f - animBlendRate_);
	}
}

void Player::UpdateAnimBlendRate()
{
	if (animBlendRate_ >= 1.0f)
	{
		return;
	}

	animBlendRate_ = min(animBlendRate_ + ANIM_BLEND_SPEED, 1.0f);
}

void Player::DrawShadow()
{
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitResDim = stage_.CollCheckCapsule(pos_, VAdd(pos_, VGet(0.0f, -SHADOW_HEIGHT, 0.0f)), SHADOW_SIZE);

	// 頂点データで変化が無い部分をセット
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// 球の直下に存在するポリゴンの数だけ繰り返し
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
	{
		// ポリゴンの座標は地面ポリゴンの座標
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ちょっと持ち上げて重ならないようにする
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

		// ポリゴンの不透明度を設定する
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > pos_.y - SHADOW_HEIGHT)
			Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - pos_.y) / SHADOW_HEIGHT);

		if (HitRes->Position[1].y > pos_.y - SHADOW_HEIGHT)
			Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - pos_.y) / SHADOW_HEIGHT);

		if (HitRes->Position[2].y > pos_.y - SHADOW_HEIGHT)
			Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - pos_.y) / SHADOW_HEIGHT);

		// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
		Vertex[0].u = (HitRes->Position[0].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pos_.x) / (SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pos_.z) / (SHADOW_SIZE * 2.0f) + 0.5f;

		// 影ポリゴンを描画
		DrawPolygon3D(Vertex, 1, shadowHandle_, TRUE);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(HitResDim);

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

}
