#pragma once
#include <type_traits>
#include <algorithm>

/// <summary>
/// 割り切れるかどうかを返す
/// </summary>
/// <typeparam name="T">整数型に限る</typeparam>
/// <param name="v">チェックしたい値</param>
/// <param name="div">モジュロ値</param>
/// <returns>true : 割り切れる</returns>
template<typename T>
inline bool IsDivisible(const T& v, const T& div)
{
	static_assert(std::is_integral<T>());
	return v % div == 0;
}

/// <summary>
/// 第一引数をインクリメントして返す
/// 第二引数以上になったときはループし0を返す
/// </summary>
/// <typeparam name="T">整数型に限る</typeparam>
/// <param name="value">現在の値</param>
/// <param name="modulo">モジュロ値</param>
/// <returns>剰余加算を行った値</returns>
template<typename T>
inline T ModuloIncrement(const T& value, const T& modulo)
{
	static_assert(std::is_integral<T>());
	return (value + 1) % modulo;
}

/// <summary>
/// 第一引数をデクリメントして返す
/// 0未満になったときはループし第二引数を返す
/// </summary>
/// <typeparam name="T">整数型に限る</typeparam>
/// <param name="value">現在の値</param>
/// <param name="modulo">モジュロ値</param>
/// <returns>剰余減算を行った値</returns>
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