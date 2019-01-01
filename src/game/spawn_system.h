#pragma once

#include <string>

#include "hooking/hooking.h"

#pragma pack(push, 1)
namespace jc
{
using spawned_objects = std::vector<std::shared_ptr<CGameObject>>;

class CSpawnSystem
{
  public:
    static CSpawnSystem& instance()
    {
        return **(CSpawnSystem**)0x142F18998;
    }

    void Spawn(const std::string& model_name, const CMatrix4f& transform,
               std::function<void(const spawned_objects&, void*)> callback, void* userdata = nullptr)
    {
        using success_t = void(__cdecl*)(const spawned_objects&, void*);

        struct SpawnReq {
            std::function<void(const spawned_objects&, void*)> callback;
            void*                                              userdata;
        };

        auto request = new SpawnReq{callback, userdata};
        hk::func_call<void>(
            0x1447C1140, this, model_name.c_str(), transform, 0x2013C,
            (success_t)[](const spawned_objects& objects, void* userdata) {
                auto spawn_req = (SpawnReq*)userdata;
                spawn_req->callback(objects, spawn_req->userdata);
                delete spawn_req;
            },
            request);
    }
};
} // namespace jc
#pragma pack(pop)
