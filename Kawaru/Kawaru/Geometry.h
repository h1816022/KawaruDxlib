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
	/// �x�N�g���̑傫����Ԃ�
	/// </summary>
	/// <returns>�x�N�g���̑傫��</returns>
	int Magnitude()const;

	/// <summary>
	/// ���K������
	/// </summary>
	void Normalize();

	/// <summary>
	/// ���K�������x�N�g����Ԃ�
	/// </summary>
	/// <returns>���K�������x�N�g��</returns>
	Vector2 Normalized()const;

	/// <summary>
	/// Vector2f�ɃL���X�g�����l��Ԃ�
	/// </summary>
	/// <returns>Vector2f�ɃL���X�g�����l</returns>
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
	/// �x�N�g���̑傫����Ԃ�
	/// </summary>
	/// <returns>�x�N�g���̑傫��</returns>
	float Magnitude()const;

	// �x�N�g���̒�����2���Ԃ�
	float SQMagnitude()const;

	/// <summary>
	/// ���K������
	/// </summary>
	void Normalize();

	/// <summary>
	/// ���K�������x�N�g����Ԃ�
	/// </summary>
	/// <returns>���K�������x�N�g��</returns>
	Vector2f Normalized()const;

	/// <summary>
	/// Vector2�ɃL���X�g�����l��Ԃ�
	/// </summary>
	/// <returns>Vector2�ɃL���X�g�����l</returns>
	Vector2 ToIntVec()const;

	/// <summary>
	/// �L���Ȓl�����ׂ�
	/// </summary>
	/// <returns>true : �L��</returns>
	bool IsValid()const;

	// �����Ȓl
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
/// ����
/// </summary>
/// <param name="va">���Ӓl</param>
/// <param name="vb">�E�Ӓl</param>
/// <returns>���ς�������</returns>
float Dot(const Vector2& va, const Vector2& vb);

/// <summary>
/// ����
/// </summary>
/// <param name="va">���Ӓl</param>
/// <param name="vb">�E�Ӓl</param>
/// <returns>���ς�������</returns>
float Dot(const Vector2f& va, const Vector2f& vb);

/// <summary>
/// �O��
/// </summary>
/// <param name="va">���Ӓl</param>
/// <param name="vb">�E�Ӓl</param>
/// <returns>�O�ς�������</returns>
float Cross(const Vector2& va, const Vector2& vb);

/// <summary>
/// �O��
/// </summary>
/// <param name="va">���Ӓl</param>
/// <param name="vb">�E�Ӓl</param>
/// <returns>�O�ς�������</returns>
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
	/// �`��
	/// </summary>
	/// <param name="color">�F��0x00000000�`0xffffffff�Ŏw�肷��</param>
	void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="offset">�ʒu�����炷���߂̃I�t�Z�b�g�l���w��</param>
	/// <param name="color">�F��0x00000000�`0xffffffff�Ŏw�肷��</param>
	void Draw(const Vector2f& offset, unsigned int color = 0xffffff)const;

	/// <summary>
	/// �߂荞�񂾋�`�̑傫����Ԃ�
	/// </summary>
	/// <param name="rcA">��`1</param>
	/// <param name="rcB">��`2</param>
	/// <returns>�߂荞�񂾋�`�̑傫��</returns>
	static Size OverlapSize(const Rect& rcA, const Rect& rcB);

	/// <summary>
	/// ��`�Ƌ�`�̏Փ˔���
	/// </summary>
	/// <param name="rcA">��`1</param>
	/// <param name="rcB">��`2</param>
	/// <returns>true : �Փ˂��Ă���</returns>
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
	/// �`��
	/// </summary>
	/// <param name="color">�F��0x00000000�`0xffffffff�Ŏw�肷��</param>
	const void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="offset">�ʒu�����炷���߂̃I�t�Z�b�g�l���w��</param>
	/// <param name="color">�F��0x00000000�`0xffffffff�Ŏw��</param>
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
	/// �X�����擾
	/// </summary>
	/// <returns>�X����</returns>
	const float GetGrad()const;

	/// <summary>
	/// �n�_����I�_�ւ̃x�N�g����Ԃ�
	/// </summary>
	/// <returns>�n�_����I�_�ւ̃x�N�g��</returns>
	const Vector2f GetVec()const;

	/// <summary>
	/// �L���Ȓl�����ׂ�
	/// </summary>
	/// <returns>true : �L��</returns>
	bool IsValid()const;

	static const Segment NIL;

	/// �����̕`��
	/// @param color �F��0x00000000�`0xffffffff�Ŏw�肷��
	const void Draw(unsigned int color = 0xffffff)const;

	/// �����̕`��
	/// @param offset �ʒu�����炷���߂̃I�t�Z�b�g�l���w��
	/// @param color �F��0x00000000�`0xffffffff�Ŏw��
	const void Draw(const Vector2& offset, unsigned int color = 0xffffff, float thickness = 1.0f)const;
};

struct Capsule
{
	/// <param name="s">�J�v�Z���̒����ɂ���������̎n�[</param>
	/// <param name="v">�J�v�Z���̒����ɓ���������̏I�[</param>
	/// <param name="r">���a</param>
	Capsule(const Position2f& s, const Position2f& v, float r) : seg(s, v), radius(r) {}

	/// <param name="s">�J�v�Z���̒����ɓ��������</param>
	/// <param name="r">���a</param>
	Capsule(const Segment& s, float r) : seg(s), radius(r) {}

	Segment seg;
	float radius;
};

struct Matrix {
	float m[3][3];
};

///�P�ʍs���Ԃ�
Matrix IdentityMat();

///���s�ړ��s���Ԃ�
Matrix TranslateMat(float x, float y);

///��]�s���Ԃ�
Matrix RotateMat(float angle);

Vector2f MultipleVec(const Matrix& mat, const Vector2f& vec);

// �s�Ϗ��� : v1��v2�͎��v���ł��邱��
//			: v1�̑傫����v2�̑傫���͓����ł��邱��
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

// �s�Ϗ��� : v1��v2�͎��v���ł��邱��
//			: v1�̑傫����v2�̑傫���͓����ł��邱��
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