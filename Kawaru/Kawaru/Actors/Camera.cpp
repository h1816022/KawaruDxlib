#include "Camera.h"
#include "../Systems/Input.h"
#include "Stage.h"

namespace
{
	// 回転速度
	constexpr float ROT_SPEED = 0.05f;

	// 注視点に設定したアクターからの高さのオフセット
	constexpr float TARGET_HEIGHT_OFFSET = 400.0f;

	// 視点座標から注視点までの距離
	constexpr float CAMERA_ARM_LENGTH = 1600.0f;

	constexpr float COLLISION_SIZE = 50.0f;
}

Camera::Camera(const Stage& stage, const float posX, const float posY, const float posZ):
	Actor(posX, posY, posZ), stage_(stage)
{
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(false);

	angleH_ = DX_PI_F;
	angleV_ = 0.0f;
}

Camera::~Camera()
{
}

void Camera::Update(const Input& input)
{
	// パッドの３ボタンか、シフトキーが押されている場合のみ角度変更操作を行う
	if (input.IsPressed("Camera"))
	{
		// 「←」ボタンが押されていたら水平角度をマイナスする
		if (input.IsPressed("Left"))
		{
			angleH_ -= ROT_SPEED;

			// －１８０度以下になったら角度値が大きくなりすぎないように３６０度を足す
			if (angleH_ < -DX_PI_F)
			{
				angleH_ += DX_TWO_PI_F;
			}
		}

		// 「→」ボタンが押されていたら水平角度をプラスする
		if (input.IsPressed("Right"))
		{
			angleH_ += ROT_SPEED;

			// １８０度以上になったら角度値が大きくなりすぎないように３６０度を引く
			if (angleH_ > DX_PI_F)
			{
				angleH_ -= DX_TWO_PI_F;
			}
		}

		// 「↑」ボタンが押されていたら垂直角度をマイナスする
		if (input.IsPressed("Up"))
		{
			angleV_ -= ROT_SPEED;

			// ある一定角度以下にはならないようにする
			if (angleV_ < -DX_PI_F / 2.0f + 0.6f)
			{
				angleV_ = -DX_PI_F / 2.0f + 0.6f;
			}
		}

		// 「↓」ボタンが押されていたら垂直角度をプラスする
		if (input.IsPressed("Down"))
		{
			angleV_ += ROT_SPEED;

			// ある一定角度以上にはならないようにする
			if (angleV_ > DX_PI_F / 2.0f - 0.6f)
			{
				angleV_ = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// カメラの注視点はプレイヤー座標から規定値分高い座標
	targetPos_ = VAdd(targetActor_->GetPos(), VGet(0.0f, TARGET_HEIGHT_OFFSET, 0.0f));

	// カメラの座標を決定する
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;

		// 水平方向の回転はＹ軸回転
		RotY = MGetRotY(angleH_);

		// 垂直方向の回転はＺ軸回転 )
		RotZ = MGetRotZ(angleV_);

		// カメラからプレイヤーまでの初期距離をセット
		Camera_Player_Length = CAMERA_ARM_LENGTH;

		// カメラの座標を算出
		// Ｘ軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
		// 垂直方向回転( Ｚ軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に
		// 注視点の座標を足したものがカメラの座標
		pos_ = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), targetPos_);

		// 注視点からカメラの座標までの間にステージのポリゴンがあるか調べる
		HRes = stage_.CollCheckCapsule(targetPos_, pos_, COLLISION_SIZE);
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0)
		{
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;

			// 当たらない位置までプレイヤーに近づく

			// ポリゴンに当たらない距離をセット
			NotHitLength = 0.0f;

			// ポリゴンに当たる距離をセット
			HitLength = Camera_Player_Length;
			do
			{
				// 当たるかどうかテストする距離をセット( 当たらない距離と当たる距離の中間 )
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;

				// テスト用のカメラ座標を算出
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), targetPos_);

				// 新しい座標で壁に当たるかテスト
				HRes = stage_.CollCheckCapsule(targetPos_, TestPosition, COLLISION_SIZE);
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0)
				{
					// 当たったら当たる距離を TestLength に変更する
					HitLength = TestLength;
				}
				else
				{
					// 当たらなかったら当たらない距離を TestLength に変更する
					NotHitLength = TestLength;
				}

				// HitLength と NoHitLength が十分に近づいていなかったらループ
			} while (HitLength - NotHitLength > 0.1f);

			// カメラの座標をセット
			pos_ = TestPosition;
		}
	}

	// カメラの情報をライブラリのカメラに反映させる
	setEye_ = VAdd(setEye_, VScale(VSub(pos_, setEye_), 0.2f));
	setTarget_ = VAdd(setTarget_, VScale(VSub(targetPos_, setTarget_), 0.2f));
	SetCameraPositionAndTarget_UpVecY(setEye_, setTarget_);
}

const VECTOR& Camera::GetTargetPos() const
{
	return targetPos_;
}

void Camera::SetTargetActor(std::shared_ptr<Actor> target)
{
	targetActor_ = target;
}
