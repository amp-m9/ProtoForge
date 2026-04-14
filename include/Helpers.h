#pragma once

template <typename T, size_t N> constexpr size_t ArrayLength(const T (&)[N])
{
    return N;
}
