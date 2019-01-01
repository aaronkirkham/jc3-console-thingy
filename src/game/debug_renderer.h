#pragma once

#include <cstdint>

#include "../vector.h"

#pragma pack(push, 1)
namespace jc
{
class PrimitiveRendererImpl
{
  public:
    void DebugRectGradient(const CVector2f &topleft, const CVector2f &bottomright, uint32_t leftcolor,
                           uint32_t rightcolor)
    {
        CVector2f p1{topleft.x, bottomright.y};
        CVector2f p0{bottomright.x, topleft.y};

        auto &leftcolor_  = CVector4f::FromARGB(leftcolor);
        auto &rightcolor_ = CVector4f::FromARGB(rightcolor);

        DebugTriangleFilled(topleft, p1, p0, leftcolor_, leftcolor_, rightcolor_);
        DebugTriangleFilled(p0, p1, bottomright, rightcolor_, leftcolor_, rightcolor_);
    }

  public:
    virtual ~PrimitiveRendererImpl()                                           = 0;
    virtual void DebugLine3d()                                                 = 0;
    virtual void DebugLine(const CVector2f &from, const CVector2f &to, const CVector4f &from_color,
                           const CVector4f &to_color)                          = 0;
    virtual void function_4()                                                  = 0;
    virtual void DebugTriangle(const CVector2f &p0, const CVector2f &p1, const CVector2f &p2, const CVector4f &c0,
                               const CVector4f &c1, const CVector4f &c2)       = 0;
    virtual void DebugTriangleFilled(const CVector2f &p0, const CVector2f &p1, const CVector2f &p2, const CVector4f &c0,
                                     const CVector4f &c1, const CVector4f &c2) = 0;
};

class CDebugRenderer
{
  public:
    static CDebugRenderer &instance()
    {
        return **(CDebugRenderer **)0x142D39DE8;
    }

  public:
    char                   _pad[0x10];
    PrimitiveRendererImpl *m_primitiveRenderer;
};
}; // namespace jc
#pragma pack(pop)
