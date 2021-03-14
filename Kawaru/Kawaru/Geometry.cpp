#include <cmath>
#include <float.h>
#include <algorithm>
#include "Geometry.h"
#include "Application.h"

const Vector2f Vector2f::NIL(NAN, NAN);

namespace
{
	VECTOR GetV2V(const Vector2f& vec)
	{
		return VGet(vec.x, vec.y, 0.0f);
	}
}

Vector2& Vector2::operator=(const Vector2& vec)
{
	this->x = vec.x;
	this->y = vec.y;
	return (*this);
}

Vector2& Vector2::operator=(const Vector2f& vec)
{
	this->x = vec.ToIntVec().x;
	this->y = vec.ToIntVec().y;
	return (*this);
}

int& Vector2::operator[](int i)
{
	if (i == 0)
	{
		return x;
	}
	else if (i == 1)
	{
		return y;
	}
	else
	{
		return x;
	}
}

bool Vector2::operator==(const Vector2& vec)
{
	return((this->x == vec.x) && (this->y == vec.y));
}

bool Vector2::operator==(const int& k)
{
	return((this->x == k) && (this->y == k));
}

bool Vector2::operator!=(const Vector2& vec)
{
	return !((this->x == vec.x) && (this->y == vec.y));
}

Vector2& Vector2::operator+=(const Vector2& vec)
{
	this->x += vec.x;
	this->y += vec.y;
	return (*this);
}

Vector2& Vector2::operator-=(const Vector2& vec)
{
	this->x -= vec.x;
	this->y -= vec.y;
	return (*this);
}

Vector2& Vector2::operator*=(int k)
{
	this->x *= k;
	this->y *= k;
	return (*this);
}

Vector2& Vector2::operator/=(int k)
{
	this->x /= k;
	this->y /= k;
	return (*this);
}

Vector2 Vector2::operator+() const
{
	return (*this);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-this->x, -this->y);
}

int Vector2::Magnitude()const
{
	return static_cast<int>(hypot(x, y));
}

void Vector2::Normalize()
{
	int mag = Magnitude();

	if (mag == 0)
	{
		return;
	}

	x /= mag;
	y /= mag;
}

Vector2 Vector2::Normalized()const
{
	int mag = Magnitude();

	if (mag == 0)
	{
		return Vector2(0, 0);
	}
	else
	{
		return Vector2(x / mag, y / mag);
	}
}

Vector2f Vector2::ToFloatVec()const
{
	return Vector2f(static_cast<float>(x), static_cast<float>(y));
}

Vector2 operator+(const Vector2& u, int k)
{
	Vector2 vec;
	vec.x = u.x + k;
	vec.y = u.y + k;
	return vec;
}

Vector2 operator+(const Vector2& u, const Vector2& v)
{
	Vector2 vec;
	vec.x = u.x + v.x;
	vec.y = u.y + v.y;
	return vec;
}

Vector2 operator-(const Vector2& u, int k)
{
	Vector2 vec;
	vec.x = u.x - k;
	vec.y = u.y - k;
	return vec;
}

Vector2 operator-(const Vector2& u, const Vector2& v)
{
	Vector2 vec;
	vec.x = u.x - v.x;
	vec.y = u.y - v.y;
	return vec;
}

Vector2 operator*(const Vector2& u, int k)
{
	Vector2 vec;
	vec.x = u.x * k;
	vec.y = u.y * k;
	return vec;
}

Vector2 operator*(const Vector2& u, const Vector2& v)
{
	Vector2 vec;
	vec.x = u.x * v.x;
	vec.y = u.y * v.y;
	return vec;
}

Vector2 operator/(const Vector2& u, int k)
{
	Vector2 vec;
	vec.x = u.x / k;
	vec.y = u.y / k;
	return vec;
}

Vector2 operator/(const Vector2& u, const Vector2& v)
{
	Vector2 vec;
	vec.x = u.x / v.x;
	vec.y = u.y / v.y;
	return vec;
}

bool operator<(const Vector2& u, const Vector2& v)
{
	return ((u.x < v.x) && (u.y < v.y));
}

bool operator<(const Vector2& u, int k)
{
	return ((u.x < k) && (u.y < k));
}

bool operator<=(const Vector2& u, const Vector2& v)
{
	return ((u.x <= v.x) && (u.y <= v.y));
}

bool operator<=(const Vector2& u, int k)
{
	return ((u.x <= k) && (u.y <= k));
}

bool operator>(const Vector2& u, const Vector2& v)
{
	return ((u.x > v.x) && (u.y > v.y));
}

bool operator>(const Vector2& u, int k)
{
	return ((u.x > k) && (u.y > k));
}

bool operator>=(const Vector2& u, const Vector2& v)
{
	return ((u.x >= v.x) && (u.y >= v.y));
}

bool operator>=(const Vector2& u, int k)
{
	return ((u.x >= k) && (u.y >= k));
}


Vector2f& Vector2f::operator=(const Vector2f& vec)
{
	this->x = vec.x;
	this->y = vec.y;
	return (*this);
}

Vector2f& Vector2f::operator=(const Vector2& vec)
{
	this->x = vec.ToFloatVec().x;
	this->y = vec.ToFloatVec().y;
	return (*this);
}

float& Vector2f::operator[](int i)
{
	if (i == 0) {
		return x;
	}
	else if (i == 1) {
		return y;
	}
	else {
		return x;
	}
}

bool Vector2f::operator==(const Vector2f& vec)
{
	return ((fabsf(this->x - vec.x) <= FLT_EPSILON) && (fabsf(this->y - vec.y) <= FLT_EPSILON));
}

bool Vector2f::operator==(const float& k)
{
	return ((fabsf(this->x - k) <= FLT_EPSILON) && (fabsf(this->y - k) <= FLT_EPSILON));
}

bool Vector2f::operator!=(const Vector2f& vec)
{
	return !((fabsf(this->x - vec.x) <= FLT_EPSILON) && (fabsf(this->y - vec.y) <= FLT_EPSILON));
}

Vector2f& Vector2f::operator+=(const Vector2f& vec)
{
	this->x += vec.x;
	this->y += vec.y;
	return (*this);
}

Vector2f& Vector2f::operator-=(const Vector2f& vec)
{
	this->x -= vec.x;
	this->y -= vec.y;
	return (*this);
}

Vector2f& Vector2f::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	return (*this);
}

Vector2f& Vector2f::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	return (*this);
}

Vector2f Vector2f::operator+() const
{
	return (*this);
}

Vector2f Vector2f::operator-() const
{
	return Vector2f(-this->x, -this->y);
}

float Vector2f::Magnitude()const
{
	return hypot(x, y);
}

float Vector2f::SQMagnitude() const
{
	return x * x + y * y;
}

void Vector2f::Normalize()
{
	float mag = Magnitude();

	if (mag == 0)
	{
		return;
	}

	x /= mag;
	y /= mag;
}

Vector2f Vector2f::Normalized()const
{
	float mag = Magnitude();

	if (mag == 0)
	{
		return Vector2f(0.0f, 0.0f);
	}
	else
	{
		return Vector2f(x / mag, y / mag);
	}
}

Vector2 Vector2f::ToIntVec()const
{
	return Vector2(static_cast<int>(x), static_cast<int>(y));
}

bool Vector2f::IsValid()const
{
	return (!isnan(this->x)) && (!isnan(this->y));
}

Vector2f operator+(const Vector2f& u, float k)
{
	Vector2f vec;
	vec.x = u.x + k;
	vec.y = u.y + k;
	return vec;
}

Vector2f operator+(const Vector2f& u, const Vector2f& v)
{
	Vector2f vec;
	vec.x = u.x + v.x;
	vec.y = u.y + v.y;
	return vec;
}

Vector2f operator-(const Vector2f& u, float k)
{
	Vector2f vec;
	vec.x = u.x - k;
	vec.y = u.y - k;
	return vec;
}

Vector2f operator-(const Vector2f& u, const Vector2f& v)
{
	Vector2f vec;
	vec.x = u.x - v.x;
	vec.y = u.y - v.y;
	return vec;
}

Vector2f operator*(const Vector2f& u, float k)
{
	Vector2f vec;
	vec.x = u.x * k;
	vec.y = u.y * k;
	return vec;
}

Vector2f operator*(const Vector2f& u, const Vector2f& v)
{
	Vector2f vec;
	vec.x = u.x * v.x;
	vec.y = u.y * v.y;
	return vec;
}

Vector2f operator/(const Vector2f& u, float k)
{
	Vector2f vec;
	vec.x = u.x / k;
	vec.y = u.y / k;
	return vec;
}

Vector2f operator/(const Vector2f& u, const Vector2f& v)
{
	Vector2f vec;
	vec.x = u.x / v.x;
	vec.y = u.y / v.y;
	return vec;
}

bool operator<(const Vector2f& u, const Vector2f& v)
{
	return ((u.x < v.x) && (u.y < v.y));
}

bool operator<(const Vector2f& u, float k)
{
	return ((u.x < k) && (u.y < k));
}

bool operator<=(const Vector2f& u, const Vector2f& v)
{
	return ((u.x <= v.x) && (u.y <= v.y));
}

bool operator<=(const Vector2f& u, float k)
{
	return ((u.x <= k) && (u.y <= k));
}

bool operator>(const Vector2f& u, const Vector2f& v)
{
	return ((u.x > v.x) && (u.y > v.y));
}

bool operator>(const Vector2f& u, float k)
{
	return ((u.x > k) && (u.y > k));
}

bool operator>=(const Vector2f& u, const Vector2f& v)
{
	return ((u.x >= v.x) && (u.y >= v.y));
}

bool operator>=(const Vector2f& u, float k)
{
	return ((u.x >= k) && (u.y >= k));
}

float Dot(const Vector2& va, const Vector2& vb)
{
	return (va.x * vb.x) + (va.y * vb.y);
}

float Dot(const Vector2f& va, const Vector2f& vb)
{
	return (va.x * vb.x) + (va.y * vb.y);
}

float Cross(const Vector2& va, const Vector2& vb)
{
	return (va.x * vb.y) - (va.y * vb.x);
}

float Cross(const Vector2f& va, const Vector2f& vb)
{
	return (va.x * vb.y) - (va.y * vb.x);
}

const int Rect::GetLeft() const
{
	return center.x - size.w / 2;
}

const int Rect::GetTop() const
{
	return center.y - size.h / 2;
}

const int Rect::GetRight() const
{
	return center.x + size.w / 2;
}

const int Rect::GetBottom() const
{
	return center.y + size.h / 2;
}

const int Rect::GetWidth() const
{
	return size.w;
}

const int Rect::GetHeight() const
{
	return size.h;
}

void Rect::Draw(unsigned int color)const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
	DrawBox(GetLeft(), GetTop(), GetRight(), GetBottom(), color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(GetLeft(), GetTop(), GetRight(), GetBottom(), color, false);
}

void Rect::Draw(const Vector2f& offset, unsigned int color)const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
	DrawBox(GetLeft() - static_cast<int>(offset.x),
		GetTop() - static_cast<int>(offset.y),
		GetRight() - static_cast<int>(offset.x),
		GetBottom() - static_cast<int>(offset.y),
		color, true);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(GetLeft() - static_cast<int>(offset.x),
		GetTop() - static_cast<int>(offset.y),
		GetRight() - static_cast<int>(offset.x),
		GetBottom() - static_cast<int>(offset.y),
		color, false);
}

Size Rect::OverlapSize(const Rect& rcA, const Rect& rcB)
{
	return Size(min(rcA.GetRight(), rcB.GetRight()) - max(rcA.GetLeft(), rcB.GetLeft()),
		min(rcA.GetBottom(), rcB.GetBottom()) - max(rcA.GetTop(), rcB.GetTop()));
}

bool Rect::IsHit(const Rect& rcA, const Rect& rcB)
{
	return (abs(rcB.center.x - rcA.center.x) < (rcA.GetWidth() + rcB.GetWidth()) / 2
		&& (abs(rcB.center.y - rcA.center.y) <= (rcA.GetHeight() + rcB.GetHeight()) / 2));
}

const void Circle::Draw(unsigned int color)const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
	DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), static_cast<int>(radius), color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), static_cast<int>(radius), color, false);
}

const void Circle::Draw(const Vector2f& offset, unsigned int color)const
{
	auto intPos = Position2(static_cast<int>(offset.x + center.x), static_cast<int>(offset.y + center.y));

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
	DrawCircle(intPos.x,
		intPos.y,
		static_cast<int>(radius),
		color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawCircle(intPos.x,
		intPos.y,
		static_cast<int>(radius),
		color, false);
}


const Segment Segment::NIL(Vector2f::NIL, Vector2f::NIL);

const float Segment::GetGrad()const
{
	if ((posB.x - posA.x) == 0.0f)
	{
		return 0.0f;
	}

	return (posB.y - posA.y) / (posB.x - posA.x);
}

const Vector2f Segment::GetVec() const
{
	return posB - posA;
}

bool Segment::IsValid() const
{
	return this->posA.IsValid() && this->posB.IsValid();
}

const void Segment::Draw(unsigned int color)const
{
	DrawLineAA(posA.x, posA.y, posB.x, posB.y, color);
}

const void Segment::Draw(const Vector2& offset, unsigned int color, float thickness)const
{
	DrawLineAA(offset.x + posA.x, offset.y + posA.y, offset.x + posB.x, offset.y + posB.y, color, thickness);
}

Size& Size::operator*=(float f)
{
	this->w *= static_cast<int>(f);
	this->h *= static_cast<int>(f);
	return (*this);
}

///単位行列を返す
Matrix IdentityMat() {
	Matrix ret = {};
	ret.m[0][0] = ret.m[1][1] = ret.m[2][2] = 1;
	return ret;
}

///平行移動行列を返す
Matrix TranslateMat(float x, float y) {
	Matrix ret = {};
	ret = IdentityMat();
	ret.m[0][2] = x;
	ret.m[1][2] = y;
	return ret;
}

///回転行列を返す
Matrix RotateMat(float angle) {
	Matrix ret = {};
	ret = IdentityMat();
	ret.m[0][0] = cos(angle);
	ret.m[0][1] = -sin(angle);
	ret.m[1][0] = sin(angle);
	ret.m[1][1] = cos(angle);
	return ret;
}

Vector2f MultipleVec(const Matrix& mat, const Vector2f& vec)
{
	Vector2f ret = {};
	ret.x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2];
	ret.y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2];
	return ret;
}

Fan::Fan(const Position2f& inCenter, const Vector2f& inV1, const Vector2f& inV2) :
	center(inCenter), v1(inV1), v2(inV2)
{
}

Fan::Fan(const Position2f& inCenter, const Vector2f& inV, float angle) :
	center(inCenter), v1(inV)
{
	v2 = MultipleVec(RotateMat(angle), inV);
}

void Fan::Draw(int graphH, int psH, unsigned int color)
{
	if (psH == -1)
	{
		std::vector<VERTEX2D> vertices;
		BuildVertex(vertices);
		DrawPrimitive2D(vertices.data(), vertices.size(), DX_PRIMTYPE_TRIANGLESTRIP, (graphH == -1) ? DX_NONE_GRAPH : graphH, false);
	}
	else
	{
		std::vector<VERTEX2DSHADER> svertices;
		BuildVertexShader(svertices);
		SetUsePixelShader(psH);
		SetUseTextureToShader(0, graphH);
		DrawPrimitive2DToShader(svertices.data(), svertices.size(), DX_PRIMTYPE_TRIANGLESTRIP);
	}

	DrawLineAA(center.x, center.y, center.x + v1.x, center.y + v1.y, 0xff0000, 1.0f);
	DrawLineAA(center.x, center.y, center.x + v2.x, center.y + v2.y, 0xff0000, 1.0f);
}

void Fan::BuildVertex(std::vector<VERTEX2D>& vertices, unsigned int color)
{
	constexpr float STEP_ANGLE = DX_PI_F / 18.0f;
	float angle = GetAngle2Vector(v1, v2);

	const int TRIANGLES_NUM = static_cast<int>(ceil(angle / STEP_ANGLE));

	vertices.resize(2 * TRIANGLES_NUM);

	for (auto& v : vertices)
	{
		v.rhw = 1.0f;
		v.dif = GetColorU8(color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff, 255);
		v.u = 0;
		v.v = 0;
	}

	auto vStart = v1;
	auto vEnd = MultipleVec(RotateMat(STEP_ANGLE), v1);

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	for (size_t i = 0; i < TRIANGLES_NUM; ++i)
	{
		vertices[i * 2 + 0].pos = GetV2V(center);
		vertices[i * 2 + 0].u = (vertices[i * 2 + 0].pos.x / vpSize.w);
		vertices[i * 2 + 0].v = (vertices[i * 2 + 0].pos.y / vpSize.h);

		vertices[i * 2 + 1].pos = GetV2V(center + vStart);
		vertices[i * 2 + 1].u = (vertices[i * 2 + 1].pos.x / vpSize.w) + cos(static_cast<float>(i + 2) / static_cast<float>(TRIANGLES_NUM)) / 50.0f;
		vertices[i * 2 + 1].v = (vertices[i * 2 + 1].pos.y / vpSize.h) + sin(static_cast<float>(i + 2) / static_cast<float>(TRIANGLES_NUM)) / 50.0f;

		vStart = vEnd;
		vEnd = MultipleVec(RotateMat(STEP_ANGLE), vStart);
	}
}

void Fan::BuildVertexShader(std::vector<VERTEX2DSHADER>& svertices, unsigned int color)
{
	constexpr float STEP_ANGLE = DX_PI_F / 72.0f;
	float angle = GetAngle2Vector(v1, v2);

	const int TRIANGLES_NUM = static_cast<int>(ceil(angle / STEP_ANGLE));

	svertices.resize(2 * TRIANGLES_NUM);

	for (auto& v : svertices)
	{
		v.rhw = 1.0f;
		v.dif = GetColorU8(color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff, 255);
		v.u = 0;
		v.v = 0;
	}

	auto vStart = v1;
	auto vEnd = MultipleVec(RotateMat(STEP_ANGLE), v1);

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	for (size_t i = 0; i < TRIANGLES_NUM; ++i)
	{
		svertices[i * 2 + 0].pos = GetV2V(center);
		svertices[i * 2 + 0].u = (svertices[i * 2 + 0].pos.x / vpSize.w);
		svertices[i * 2 + 0].v = (svertices[i * 2 + 0].pos.y / vpSize.h);

		svertices[i * 2 + 1].pos = GetV2V(center + vStart);
		svertices[i * 2 + 1].u = (svertices[i * 2 + 1].pos.x / vpSize.w)/* + cos(static_cast<float>(i + 2) / static_cast<float>(TRIANGLES_NUM)) / 50.0f*/;
		svertices[i * 2 + 1].v = (svertices[i * 2 + 1].pos.y / vpSize.h)/* + sin(static_cast<float>(i + 2) / static_cast<float>(TRIANGLES_NUM)) / 50.0f*/;

		vStart = vEnd;
		vEnd = MultipleVec(RotateMat(STEP_ANGLE), vStart);
	}
}

float Fan::GetRadius() const
{
	return v1.Magnitude();
}

void Fan::AddAngle1(float angle)
{
	float tmpAngle = GetAngle2Vector(v1, v2);
	v2 = MultipleVec(RotateMat(tmpAngle + angle), v1);
}

void Fan::AddAngle2(float angle)
{
	float tmpAngle = GetAngle2Vector(v2, v1);
	v1 = MultipleVec(RotateMat(tmpAngle + angle), v2);
}

float Fan::GetAngle() const
{
	return acos(Dot(v1, v2) / v1.Magnitude() * v2.Magnitude());
}

float GetAngle2Vector(const Vector2f& v1, const Vector2f& v2)
{
	auto tmpV1 = v1.Normalized();
	auto tmpV2 = v2.Normalized();

	float dot = Dot(tmpV1, tmpV2);
	float cross = Cross(tmpV1, tmpV2);
	auto angle = atan2f(cross, dot);

	if (angle < 0.0f)
	{
		angle += DX_PI_F * 2.0f;
	}

	return angle;
}

bool CheckMatch(const VECTOR& v1, const VECTOR& v2)
{
	const float ALLOWABLE_ERROR = 0.1f;

	return (abs(v1.x - v2.x) < ALLOWABLE_ERROR) && (abs(v1.y - v2.y) < ALLOWABLE_ERROR) && (abs(v1.z - v2.z) < ALLOWABLE_ERROR);
}

bool CheckMatch2D(const VECTOR& v1, const VECTOR& v2)
{
	return CheckMatch(VGet(v1.x, 0.0f, v1.z), VGet(v2.x, 0.0f, v2.z));
}

float GetLength(const VECTOR& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

VECTOR Lerp(const VECTOR& v1, const VECTOR& v2, float rate)
{
	rate = std::clamp(rate, 0.0f, 1.0f);
	return VAdd(VScale(v1, (1.0f - rate)), VScale(v2, rate));
}

VECTOR RandomVector()
{
	int x, y, z;

	x = GetRand(199) + 1;
	y = GetRand(199) + 1;
	z = GetRand(199) + 1;

	return VNorm(VGet(1.0f - static_cast<float>(x) / 100.0f, 1.0f - static_cast<float>(y) / 100.0f, 1.0f - static_cast<float>(z) / 100.0f));
}

float Lerp(float f1, float f2, float rate)
{
	rate = std::clamp(rate, 0.0f, 1.0f);
	return f1 * (1.0f - rate) + f2 * rate;
}

SlashShape::SlashShape(const Position2f& inCenter, const Vector2f& inV1, const Vector2f& inV2) :
	center(inCenter), v1(inV1), v2(inV2)
{
}

SlashShape::SlashShape(const Position2f& inCenter, const Vector2f& inV, float angle) :
	center(inCenter), v1(inV)
{
	v2 = MultipleVec(RotateMat(angle), inV);
}

void SlashShape::AddAngle1(float angle)
{
	float tmpAngle = GetAngle2Vector(v1, v2);
	v2 = MultipleVec(RotateMat(tmpAngle + angle), v1);
}

void SlashShape::AddAngle2(float angle)
{
	float tmpAngle = GetAngle2Vector(v2, v1);
	v1 = MultipleVec(RotateMat(tmpAngle + angle), v2);
}

void SlashShape::SetAngle1(float angle)
{
	v1 = MultipleVec(RotateMat(angle), v2);
}

void SlashShape::SetAngle2(float angle)
{
	v2 = MultipleVec(RotateMat(angle), v1);
}

void SlashShape::Draw(int graphH, int normH, int psH, float amp)
{
	if (psH == -1)
	{
		std::vector<VERTEX2D> vertices;
		BuildVertex(vertices);
		DrawPrimitive2D(vertices.data(), vertices.size(), DX_PRIMTYPE_TRIANGLESTRIP, (graphH == -1) ? DX_NONE_GRAPH : graphH, false);
	}
	else
	{
		std::vector<VERTEX2DSHADER> svertices;
		BuildVertexShader(svertices);
		SetUsePixelShader(psH);
		SetUseTextureToShader(0, graphH);

		if (normH != -1)
		{
			SetUseTextureToShader(1, normH);
		}

		DrawPrimitive2DToShader(svertices.data(), svertices.size(), DX_PRIMTYPE_TRIANGLESTRIP);
	}

	DrawLineAA(center.x, center.y, center.x + v1.x, center.y + v1.y, 0xff0000, 1.0f);
	DrawLineAA(center.x, center.y, center.x + v2.x, center.y + v2.y, 0xff0000, 1.0f);
}

void SlashShape::BuildVertex(std::vector<VERTEX2D>& vertices, unsigned int color)
{
	constexpr float STEP_ANGLE = DX_PI_F / 72.0f;
	float angle = GetAngle2Vector(v1, v2);

	const int TRIANGLES_NUM = static_cast<int>(ceil(angle / STEP_ANGLE));

	vertices.resize(2 * TRIANGLES_NUM);

	for (auto& v : vertices)
	{
		v.rhw = 1.0f;
		v.dif = GetColorU8(color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff, 255);
		v.u = 0;
		v.v = 0;
	}

	auto vStart = v1;
	auto vEnd = MultipleVec(RotateMat(STEP_ANGLE), v1);

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	float curAngle = 0.0f;
	Vector2f tmpV = v1;

	for (size_t i = 0; i < TRIANGLES_NUM; ++i)
	{
		auto ratio = curAngle / angle;

		auto inr = GetRadius() * (1 - ratio);

		auto nv = tmpV.Normalized();

		vertices[i * 2 + 0].pos = GetV2V(center + nv * inr);
		vertices[i * 2 + 0].u = vertices[i * 2 + 0].pos.x / vpSize.w;
		vertices[i * 2 + 0].v = vertices[i * 2 + 0].pos.y / vpSize.h;

		vertices[i * 2 + 1].pos = GetV2V(center + nv * GetRadius());
		vertices[i * 2 + 1].u = vertices[i * 2 + 1].pos.x / vpSize.w;
		vertices[i * 2 + 1].v = vertices[i * 2 + 1].pos.y / vpSize.h;

		curAngle += STEP_ANGLE;
		tmpV = MultipleVec(RotateMat(STEP_ANGLE), tmpV);
	}
}

void SlashShape::BuildVertexShader(std::vector<VERTEX2DSHADER>& svertices, unsigned int color)
{
	constexpr float STEP_ANGLE = DX_PI_F / 72.0f;
	float angle = GetAngle2Vector(v1, v2);

	const int TRIANGLES_NUM = static_cast<int>(ceil(angle / STEP_ANGLE));

	svertices.resize(2 * TRIANGLES_NUM);

	for (auto& v : svertices)
	{
		v.rhw = 1.0f;
		v.dif = GetColorU8(color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff, 255);
		v.u = 0;
		v.v = 0;
	}

	auto vStart = v1;
	auto vEnd = MultipleVec(RotateMat(STEP_ANGLE), v1);

	const auto& vpSize = Application::Instance().GetViewport().GetSize();

	float curAngle = 0.0f;
	Vector2f tmpV = v1;

	for (size_t i = 0; i < TRIANGLES_NUM; ++i)
	{
		auto ratio = curAngle / angle;

		auto inr = GetRadius() * (1 - ratio);

		auto nv = tmpV.Normalized();

		svertices[i * 2 + 0].pos = GetV2V(center + nv * inr);
		svertices[i * 2 + 0].u = svertices[i * 2 + 0].pos.x / vpSize.w;
		svertices[i * 2 + 0].v = svertices[i * 2 + 0].pos.y / vpSize.h;

		svertices[i * 2 + 1].pos = GetV2V(center + nv * GetRadius());
		svertices[i * 2 + 1].u = svertices[i * 2 + 1].pos.x / vpSize.w;
		svertices[i * 2 + 1].v = svertices[i * 2 + 1].pos.y / vpSize.h;

		curAngle += STEP_ANGLE;
		tmpV = MultipleVec(RotateMat(STEP_ANGLE), tmpV);
	}
}

float SlashShape::GetRadius()
{
	return v1.Magnitude();
}

float SlashShape::GetAngle()
{
	return acos(Dot(v1, v2) / v1.Magnitude() * v2.Magnitude());
}
