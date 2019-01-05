#pragma once

#include "command.h"

#include <set>

class SpawnCommand : public ICommand
{
  private:
#pragma pack(push, 1)
    struct PropertyFileEntry {
        uint32_t hash;
        uint32_t data;
        uint8_t  type;
    };

    struct PropertyFileResult {
        uint8_t *          base;
        PropertyFileEntry *entry;
        uint8_t            error;
    };
#pragma pack(pop)

  public:
    inline static std::set<std::string> m_Hints = {};

    virtual const char *GetCommand() override
    {
        return "spawn";
    }

    virtual void Initialize() override
    {
        static hk::inject_call<PropertyFileResult *, void *, PropertyFileResult *, uint32_t> add_event_hook(
            0x1447EC034);
        add_event_hook.inject([](void *file, PropertyFileResult *buf, uint32_t hash) {
            PropertyFileResult model;

            add_event_hook.call(file, &model, 0xF71C2A21);

            if (model.entry) {
                m_Hints.insert((char *)(model.base + model.entry->data));
            }

            return add_event_hook.call(file, buf, hash);
        });
    }

    virtual bool Handler(const std::string &arguments) override
    {
        auto  local_player = jc::CNetworkPlayerManager::instance().m_localPlayer->m_player;
        auto  transform    = local_player->m_character->m_transform;
        auto &aimpos       = local_player->m_aimControl->m_aimPos;

        transform.m[3].x = aimpos.x;
        transform.m[3].y = aimpos.y + 1.0f;
        transform.m[3].z = aimpos.z;

        jc::CSpawnSystem::instance().Spawn(arguments, transform, [](const jc::spawned_objects &objects, void *) {
            //
        });

        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string &arguments) override
    {
        std::vector<std::string> result;
        std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(result),
                     [&](const std::string &item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
