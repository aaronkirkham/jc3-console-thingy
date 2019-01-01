#pragma once

struct ID3D11Device;

#pragma pack(push, 1)
namespace jc
{
class HDevice_t
{
  public:
    char                 _pad[0x28];
    ID3D11Device*        m_device;
    char                 _pad2[0x170];
    int32_t              m_screenWidth;
    int32_t              m_screenHeight;
};
}; // namespace jc
#pragma pack(pop)
