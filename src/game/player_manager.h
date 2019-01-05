#pragma once

#include "player.h"

#pragma pack(push, 1)
namespace jc
{
class CNetworkPlayer
{
  public:
    char                        _pad[0x128];
    std::shared_ptr<CPlayer>    m_player;
    std::shared_ptr<CCharacter> m_character;
};

class CNetworkPlayerManager
{
  public:
    static CNetworkPlayerManager& instance()
    {
        return **(CNetworkPlayerManager**)0x142F36958;
    }

    static CCharacter* GetLocalPlayerCharacter()
    {
        auto inst = &instance();
        if (inst && inst->m_localPlayer && inst->m_localPlayer->m_character) {
            return inst->m_localPlayer->m_character.get();
        }

        return nullptr;
    }

  public:
    char            _pad[0x48];
    CNetworkPlayer* m_localPlayer;
};
}; // namespace jc
#pragma pack(pop)
