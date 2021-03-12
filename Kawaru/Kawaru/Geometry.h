#pragma once
#include <vector>
#include <DxLib.h>

struct Size
{
	int w, h;

	Size() :w(), h() {};
	Size(int inx, int iny) :w(inx), h(iny) {};

	Size& operator*=(float f);

};

struct Vector2f;

struct Vector2
{
	int x, y;
	Vector2() :x(), y() {};
	Vector2(int inx, int iny) :x(inx), y(iny) {};

	Vector2& operator=(const Vector2& vec);
	Vector2& operator=(const Vector2f& vec);

	int& operator[](int i);

	bool operator==(const Vector2& vec);
	bool operator==(const int& k);
	bool operator!=(const Vector2& vec);

	Vector2& operator+=(const Vector2& vec);
	Vector2& operator-=(const Vector2& vec);
	Vector2& operator*=(int k);
	Vector2& operator/=(int k);
	Vector2 operator+()const;
	Vector2 operator-()const;

	/// <summary>
	/// ベクトルの大きさを返す
	/// </summary>
	/// <returns>ベクトルの大きさ</returns>
	int Magnitude()const;

	/// <summary>
	/// 正規化する
	/// </summary>
	void Normalize();

	/// <summary>
	/// 正規化したベクトルを返す
	/// </summary>
	/// <returns>正規化したベクトル</returns>
	Vector2 Normalized()const;

	/// <summary>
	/// Vector2fにキャストした値を返す
	/// </summary>
	/// <returns>Vector2fにキャストした値</returns>
	Vector2f ToFloatVec()const;
};

struct Vector2f
{
	float x, y;
	Vector2f() :x(), y() {};
	Vector2f(float inx, float iny) :x(inx), y(iny) {};

	Vector2f& operator=(const Vector2f& vec);
	Vector2f& operator=(const Vector2& vec);

	float& operator[](int i);

	bool operator==(const Vector2f& vec);
	bool operator==(const float& k);
	bool operator!=(const Vector2f& vec);

	Vector2f& operator+=(const Vector2f& vec);
	Vector2f& operator-=(const Vector2f& vec);
	Vector2f& operator*=(float k);
	Vector2f& operator/=(float k);
	Vector2f operator+()const;
	Vector2f operator-()const;

	/// <summary>
	/// ベクトルの大きさを返す
	/// </summary>
	/// <returns>ベクトルの大きさ</returns>
	float Magnitude()const;

	// ベクトルの長さの2乗を返す
	float SQMagnitude()const;

	/// <summary>
	/// 正規化する
	/// </summary>
	void Normalize();

	/// <summary>
	/// 正規化したベクトルを返す
	/// </summary>
	/// <returns>正規化したベクトル</returns>
	Vector2f Normalized()const;

	/// <summary>
	/// Vector2にキャストした値を返す
	/// </summary>
	/// <returns>Vector2にキャストした値</returns>
	Vector2 ToIntVec()const;

	/// <summary>
	/// 有効な値か調べる
	/// </summary>
	/// <returns>true : 有効</returns>
	bool IsValid()const;

	// 無効な値
	static const Vector2f NIL;
};

using Position2 = Vector2;
using Position2f = Vector2f;

Vector2 operator+(const Vector2& u, int k);
Vector2 operator+(const Vector2& u, const Vector2& v);
Vector2 operator-(const Vector2& u, int k);
Vector2 operator-(const Vector2& u, const Vector2& v);
Vector2 operator*(const Vector2& u, int k);
Vector2 operator*(const Vector2& u, const Vector2& v);
Vector2 operator/(const Vector2& u, int k);
Vector2 operator/(const Vector2& u, const Vector2& v);

bool operator<(const Vector2& u, const Vector2& v);
bool operator<(const Vector2& u, int k);
bool operator<=(const Vector2& u, const Vector2& v);
bool operator<=(const Vector2& u, int k);
bool operator>(const Vector2& u, const Vector2& v);
bool operator>(const Vector2& u, int k);
bool operator>=(const Vector2& u, const Vector2& v);
bool operator>=(const Vector2& u, int k);

Vector2f operator+(const Vector2f& u, float k);
Vector2f operator+(const Vector2f& u, const Vector2f& v);
Vector2f operator-(const Vector2f& u, float k);
Vector2f operator-(const Vector2f& u, const Vector2f& v);
Vector2f operator*(const Vector2f& u, float k);
Vector2f operator*(const Vector2f& u, const Vector2f& v);
Vector2f operator/(const Vector2f& u, float k);
Vector2f operator/(const Vector2f& u, const Vector2f& v);

bool operator<(const Vector2f& u, const Vector2f& v);
bool operator<(const Vector2f& u, float k);
bool operator<=(const Vector2f& u, const Vector2f& v);
bool operator<=(const Vector2f& u, float k);
bool operator>(const Vector2f& u, const Vector2f& v);
bool operator>(const Vector2f& u, float k);
bool operator>=(const Vector2f& u, const Vector2f& v);
bool operator>=(const Vector2f& u, float k);

/// <summary>
/// 内積
/// </summary>
/// <param name="va">左辺値</param>
/// <param name="vb">右辺値</param>
/// <returns>内積した結果</returns>
float Dot(const Vector2& va, const Vector2& vb);

/// <summary>
/// 内積
/// </summary>
/// <param name="va">左辺値</param>
/// <param name="vb">右辺値</param>
/// <returns>内積した結果</returns>
float Dot(const Vector2f& va, const Vector2f& vb);

/// <summary>
/// 外積
/// </summary>
/// <param name="va">左辺値</param>
/// <param name="vb">右辺値</param>
/// <returns>外積した結果</returns>
float Cross(const Vector2& va, const Vector2& vb);

/// <summary>
/// 外積
/// </summary>
/// <param name="va">左辺値</param>
/// <param name="vb">右辺値</param>
/// <returns>外積した結果</returns>
float Cross(const Vector2f& va, const Vector2f& vb);

struct Rect
{
	Position2 center;
	Size size;
	Rect() :center(), size() {};
	Rect(int x, int y, int w, int h) :center(x, y), size(w, h) {};
	Rect(const Position2& pos, const Size& sz) :center(pos), size(sz) {};
	Rect(const Position2f& pos, const Size& sz) :center(pos.ToIntVec()), size(sz) {};
	const int GetLeft()const;
	const int GetTop()const;
	const int GetRight()const;
	const int GetBottom()const;
	const int GetWidth()const;
	const int GetHeight()const;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="color">色を0x00000000～0xffffffffで指定する</param>
	void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="offset">位置をずらすためのオフセット値を指定</param>
	/// <param name="color">色を0x00000000～0xffffffffで指定する</param>
	void Draw(const Vector2f& offset, unsigned int color = 0xffffff)const;

	/// <summary>
	/// めり込んだ矩形の大きさを返す
	/// </summary>
	/// <param name="rcA">矩形1</param>
	/// <param name="rcB">矩形2</param>
	/// <returns>めり込んだ矩形の大きさ</returns>
	static Size OverlapSize(const Rect& rcA, const Rect& rcB);

	/// <summary>
	/// 矩形と矩形の衝突判定
	/// </summary>
	/// <param name="rcA">矩形1</param>
	/// <param name="rcB">矩形2</param>
	/// <returns>true : 衝突している</returns>
	static bool IsHit(const Rect& rcA, const Rect& rcB);
};

struct Circle
{
	Position2f center;
	float radius;
	Circle() :center(), radius() {};
	Circle(float x, float y, float r) :center(x, y), radius(r) {};
	Circle(const Position2f& p, float r) :center(p), radius(r) {};

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="color">色を0x00000000～0xffffffffで指定する</param>
	const void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="offset">位置をずらすためのオフセット値を指定</param>
	/// <param name="color">色を0x00000000～0xffffffffで指定</param>
	const void Draw(const Vector2f& offset, unsigned int color = 0xffffff)const;
};

struct Segment
{
	Position2f posA;
	Position2f posB;
	Segment() :posA(), posB() {};
	Segment(float ax, float ay, float bx, float by) :posA(ax, ay), posB(bx, by) {};
	Segment(const Position2f& a, const Position2f& b) :posA(a), posB(b) {};

	/// <summary>
	/// 傾きを取得
	/// </summary>
	/// <returns>傾き量</returns>
	const float GetGrad()const;

	/// <summary>
	/// 始点から終点へのベクトルを返す
	/// </summary>
	/// <returns>始点から終点へのベクトル</returns>
	const Vector2f GetVec()const;

	/// <summary>
	/// 有効な値か調べる
	/// </summary>
	/// <returns>true : 有効</returns>
	bool IsValid()const;

	static const Segment NIL;

	/// 線分の描画
	/// @param color 色を0x00000000～0xffffffffで指定する
	const void Draw(unsigned int color = 0xffffff)const;

	/// 線分の描画
	/// @param offset 位置をずらすためのオフセット値を指定
	/// @param color 色を0x00000000～0xffffffffで指定
	const void Draw(const Vector2& offset, unsigned int color = 0xffffff, float thickness = 1.0f)const;
};

struct Capsule
{
	/// <param name="s">カプセルの長さにあたる線分の始端</param>
	/// <param name="v">カプセルの長さに当たる線分の終端</param>
	/// <param name="r">半径</param>
	Capsule(const Position2f& s, const Position2f& v, float r) : seg(s, v), radius(r) {}

	/// <param name="s">カプセルの長さに当たる線分</param>
	/// <param name="r">半径</param>
	Capsule(const Segment& s, float r) : seg(s), radius(r) {}

	Segment seg;
	float radius;
};

struct Matrix {
	float m[3][3];
};

///単位行列を返す
Matrix IdentityMat();

///平行移動行列を返す
Matrix TranslateMat(float x, float y);

///回転行列を返す
Matrix RotateMat(float angle);

Vector2f MultipleVec(const Matrix& mat, const Vector2f& vec);

// 不変条件 : v1→v2は時計回りであること
//			: v1の大きさとv2の大きさは同じであること
struct Fan
{
	Fan() : center(0.0f, 0.0f), v1(0.0f, 0.0f), v2(0.0f, 0.0f) {}
	Fan(const Position2f& inCenter, const Vector2f& inV1, const Vector2f& inV2);
	Fan(const Position2f& inCenter, const Vector2f& inV, float angle);

	void Draw(int graphH = -1, int psH = -1, unsigned int color = 0xffffff);

	void BuildVertex(std::vector<VERTEX2D>& vertices, unsigned int color = 0xffffff);

	void BuildVertexShader(std::vector<VERTEX2DSHADER>& svertices, unsigned int color = 0xffffff);

	float GetRadius()const;

	void AddAngle1(float angle);
	void AddAngle2(float angle);

	float GetAngle()const;

	Position2f center;
	Vector2f v1;
	Vector2f v2;
};

float GetAngle2Vector(const Vector2f& v1, const Vector2f& v2);

// 不変条件 : v1→v2は時計回りであること
//			: v1の大きさとv2の大きさは同じであること
struct SlashShape
{
	SlashShape(const Position2f& inCenter, const Vector2f& inV1, const Vector2f& inV2);
	SlashShape(const Position2f& inCenter, const Vector2f& inV, float angle);

	void AddAngle1(float angle);
	void AddAngle2(float angle);

	void SetAngle1(float angle);
	void SetAngle2(float angle);

	void Draw(int graphH = -1, int psH = -1, int normH = -1, float amp = 0.0f);

	void BuildVertex(std::vector<VERTEX2D>& vertices, unsigned int color = 0xffffff);
	void BuildVertexShader(std::vector<VERTEX2DSHADER>& svertices, unsigned int color = 0xffffff);

	float GetRadius();

	float GetAngle();

	Position2f center;
	Vector2f v1;
	Vector2f v2;
};

bool CheckMatch(const VECTOR& v1, const VECTOR& v2);
bool CheckMatch2D(const VECTOR& v1, const VECTOR& v2);