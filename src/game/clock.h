#pragma once

#pragma pack(push, 1)
namespace jc::Base
{
class CClock
{
  public:
    static CClock& instance()
    {
        return **(CClock**)0x142ED0E78;
    }

  public:
    char _pad[0x2C];
    bool m_paused;
};
}; // namespace jc::Base
#pragma pack(pop)
