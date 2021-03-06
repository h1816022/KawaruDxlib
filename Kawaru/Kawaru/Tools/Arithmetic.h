#pragma once
#include <type_traits>
#include <algorithm>

/// <summary>
/// ����؂�邩�ǂ�����Ԃ�
/// </summary>
/// <typeparam name="T">�����^�Ɍ���</typeparam>
/// <param name="v">�`�F�b�N�������l</param>
/// <param name="div">���W�����l</param>
/// <returns>true : ����؂��</returns>
template<typename T>
inline bool IsDivisible(const T& v, const T& div)
{
	static_assert(std::is_integral<T>());
	return v % div == 0;
}

/// <summary>
/// ���������C���N�������g���ĕԂ�
/// �������ȏ�ɂȂ����Ƃ��̓��[�v��0��Ԃ�
/// </summary>
/// <typeparam name="T">�����^�Ɍ���</typeparam>
/// <param name="value">���݂̒l</param>
/// <param name="modulo">���W�����l</param>
/// <returns>��]���Z���s�����l</returns>
template<typename T>
inline T ModuloIncrement(const T& value, const T& modulo)
{
	static_assert(std::is_integral<T>());
	return (value + 1) % modulo;
}

/// <summary>
/// ���������f�N�������g���ĕԂ�
/// 0�����ɂȂ����Ƃ��̓��[�v����������Ԃ�
/// </summary>
/// <typeparam name="T">�����^�Ɍ���</typeparam>
/// <param name="value">���݂̒l</param>
/// <param name="modulo">���W�����l</param>
/// <returns>��]���Z���s�����l</returns>
template<typename T>
inline T ModuloDecrement(const T& value, const T& modulo)
{
	static_assert(std::is_integral<T>());
	return (value - 1 + modulo) % modulo;
}

template<typename T>
inline T Lerp(const T& v1, const T& v2, float rate)
{
	rate = std::clamp(rate, 0.0f, 1.0f);
	return v1 * rate + v2 * (1.0f - rate);
}