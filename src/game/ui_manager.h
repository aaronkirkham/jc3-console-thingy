#pragma once

#pragma pack(push, 1)
namespace jc
{
class CUIManager
{
  public:
    static CUIManager& instance()
    {
        return **(CUIManager**)0x142E2B700;
    }

  public:
    char _pad[0x213];
    bool m_active;
};
}; // namespace jc
#pragma pack(pop)
