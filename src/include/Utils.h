#include <cstddef>
template <typename T, size_t N> // returns size of C-style arrays
unsigned int Length(T (&Array)[N])
{
    if (sizeof(Array))
    {
        return sizeof(Array) / sizeof(*Array);
    }

    return 0;
}