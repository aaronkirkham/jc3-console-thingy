#pragma once

#pragma pack(push, 1)
namespace jc::Input
{
class CInputDeviceManager
{
  public:
    static CInputDeviceManager& instance()
    {
        return **(CInputDeviceManager**)0x142E2B6F8;
    }

  public:
    char _pad[0x70];
    bool m_hasFocus;
};
}; // namespace jc::Input
#pragma pack(pop)
