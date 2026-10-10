#pragma once

#include <array>

enum class RotationType : std::uint8_t 
{
    Quaternion,
    XYZ,
    XZY,
    YXZ,
    YZX,
    ZXY,
    ZYX,
    AxisAngle,
};

inline std::array<std::uint8_t, 3> GetRotationOrderFromEnum(const RotationType Type)
{
    switch (Type)
    {
    case RotationType::XYZ:
        return{2, 1, 0};
        
    case RotationType::XZY:
        return{1, 2, 0};
    
    case RotationType::YXZ:
        return{2, 0, 1};
    
    case RotationType::YZX:
        return{0, 2, 1};
    
    case RotationType::ZXY:
        return{1, 0, 2};

    case RotationType::ZYX:
        return{0, 1, 2};

    case RotationType::Quaternion:
    case RotationType::AxisAngle:
    default:
        return {0,0,0};
    }
}