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

        // figure out if the player wants to spawn more than one thing
        if (arguments.find("*", 0) != std::string::npos) {
            // identify the multiplier and turn it into something we can use, then create a string without it
            // also identify the spacer to enable spawning of bigger things
            // limiting the size of the substring limits max spawned entities, although the game seems to limit this
            // itself
            size_t      multiplier_position = arguments.find("*", 0);
            size_t      spacer_position     = arguments.find("/", 0);
            std::string truncated_args      = arguments.substr(0, multiplier_position - 1);

            int multiplier = std::stoi(arguments.substr(multiplier_position + 1, 4));
            int spacer     = 5;

            if (spacer_position != std::string::npos) {
                spacer = std::stoi(arguments.substr(spacer_position + 1, std::string::npos));
            }

            int square_root = (int)(std::sqrt((double)multiplier) + 0.5f);
            // do the actual spawning
            for (int i = 0; i < multiplier; i++) {
                // spawn things in as square a rectangle as possible
                if ((i % square_root == 0) && (i != 0)) {
                    transform.m[3].x = transform.m[3].x + spacer;
                    transform.m[3].z = transform.m[3].z - (square_root * spacer);
                } else {
                    transform.m[3].z = transform.m[3].z + spacer;
                }
                jc::CSpawnSystem::instance().Spawn(truncated_args, transform,
                                                   [](const jc::spawned_objects &objects, void *) {});
            }

        } else {

            jc::CSpawnSystem::instance().Spawn(arguments, transform, [](const jc::spawned_objects &objects, void *) {});
        }

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
