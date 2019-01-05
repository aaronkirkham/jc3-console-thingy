#pragma once

#include "character.h"

#pragma pack(push, 1)
namespace jc
{
class CPlayerAimControl
{
  public:
    char      _pad[0x14C];
    CVector3f m_aimPos;
};

class CPlayer
{
  public:
    // NOTE(aaron): everything here is out of alignment by 16 bytes because we
    // don't use CAvatar in CNetworkPlayer
    char               _pad[0x100];
    CCharacter*        m_character;
    char               _pad2[0x40];
    CPlayerAimControl* m_aimControl;
};
}; // namespace jc
#pragma pack(pop)
