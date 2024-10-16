#pragma once
#include <string>
#include <vector>
#include "Settings.h"



class Utility : public Singleton<Utility>
{
public:    

    inline static std::vector<RE::Actor*> GetNearbyActors(RE::TESObjectREFR* a_ref, float a_radius, bool a_ignorePlayer)
    {
        {
            std::vector<RE::Actor*> result;
            if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
                if (a_ignorePlayer && processLists->numberHighActors == 0) {
                    logger::debug("no process list");
                    return result;
                }

                const auto squaredRadius = a_radius * a_radius;
                const auto originPos     = a_ref->GetPosition();

                result.reserve(processLists->numberHighActors);

                const auto get_actor_within_radius = [&](RE::Actor* a_actor) {
                    if (a_actor && a_actor != a_ref && originPos.GetSquaredDistance(a_actor->GetPosition()) <= squaredRadius) {
                        result.emplace_back(a_actor);
                    }
                    };
                for (auto& actorHandle : processLists->highActorHandles) {
                    const auto actor = actorHandle.get();
                    get_actor_within_radius(actor.get());
                }

                if (!a_ignorePlayer) {
                    get_actor_within_radius(RE::PlayerCharacter::GetSingleton());
                }

                if (!result.empty()) {
                    logger::debug("vector is not empty");
                    return result;
                }
            }
            return result;
        }
    }


    inline static int RandomInt(int a_max)
    {
        static std::random_device       rd;
        static std::mt19937             gen(rd());
        std::uniform_int_distribution<> distrib(0, a_max);
        logger::debug("random number is {} between {} and {}", distrib(gen), 0, a_max);
        return distrib(gen);
    }

    inline static float GetMaxHealthActor(RE::Actor* a_actor)
    {

        return a_actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kHealth)
               + a_actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kHealth);
    }

    inline static float GetMaxStamina(RE::Actor* actor)
    {
        return actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, RE::ActorValue::kStamina) + actor->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina);
    }

    static bool RestrictedNames(RE::TESObjectCELL* cell) {
        const char* wild = "Wilderness";
        const char* wild2 = "Wilderness ";
        const char* empty = "";
        const char* empty2 = " ";

        if (cell->GetFormEditorID() == wild2) {
            return true;
        }
        if (cell->GetFormEditorID() == wild) {
            return true;
        }
        if (cell->GetFormEditorID() == empty) {
            return true;
        }
        if (cell->GetFormEditorID() == empty2) {
            return true;
        }
        else
            return false; 

    }

    inline static std::vector<RE::TESObjectCELL*> cell_vec;

    inline static void printCell(RE::TESObjectCELL* cell) {
        if (cell) {
            logger::debug("selected cell is {}", cell->GetName());
        }
        else
            logger::debug("cell is nullptr");
        
    }

    static RE::TESObjectCELL* findRandomCell(RE::TESWorldSpace* world, RE::PlayerCharacter* player) {
        Settings* settings = Settings::GetSingleton();
        std::vector<RE::TESObjectCELL*> cell_vec{};
        cell_vec.clear();
        for (auto& [ID, cell] : world->cellMap) {
            if (std::strcmp(cell->GetFormEditorID(), "Wilderness") != 0 && player->GetParentCell() != cell ) {
                auto distance = player->GetPlayerRuntimeData().exteriorPosition.GetDistance(RE::NiPoint3(cell->GetCoordinates()->worldX, cell->GetCoordinates()->worldY, cell->GetExteriorWaterHeight()));
                if (distance < settings->maxScanDist) {
                    if (cell == player->GetPlayerRuntimeData().lastKnownGoodLocation || cell->GetLocation() == player->GetPlayerRuntimeData().lastKnownGoodLocation) {
                        logger::debug("found known cell, teleport to that");
                        return cell;
                    }
                    cell_vec.push_back(cell);
                }      
            }
        }
        if (!cell_vec.empty()) {
            int randomCell = RandomInt(cell_vec.size() - 1);
            logger::debug("other world vector has {} entries", cell_vec.size());
            return cell_vec[randomCell];
        }
        return nullptr;
        

    }

    inline static void PacifyEnemies(RE::TESObjectCELL* a_cell) {
        for (auto& cell : a_cell->GetRuntimeData().references) {
            if (cell) {
                if (auto actor = cell.get()->As<RE::Actor>(); actor && actor->IsInCombat()) {
                    logger::debug("found NPC ref it is: {}", actor->GetName());
                    const auto process = RE::ProcessLists::GetSingleton();
                    process->runDetection = false;
                    process->ClearCachedFactionFightReactions();
                    process->StopCombatAndAlarmOnActor(actor, false);
                    process->runDetection = true;
                    actor->StopCombat();
                }
            }
        }
    }

    inline static void TeleportPlayer(RE::PlayerCharacter* a_actor) {
        auto a_cell = a_actor->GetParentCell();
        Settings* settings = Settings::GetSingleton();

        if (!a_cell) {
            a_cell = a_actor->GetSaveParentCell();
        }

        if (a_cell && a_cell->IsInteriorCell()) {
            logger::debug("is interior cell");
            for (auto& cell : a_cell->GetRuntimeData().references) {
                if (cell) {
                    if (auto actor = cell.get()->As<RE::Actor>(); actor && actor->IsInCombat()) {
                        logger::debug("found NPC ref it is: {}", actor->GetName());
                        const auto process = RE::ProcessLists::GetSingleton();
                        process->runDetection = false;
                        process->ClearCachedFactionFightReactions();
                        process->StopCombatAndAlarmOnActor(actor, false);
                        process->runDetection = true;
                        actor->StopCombat();
                    }
                }
            }
            a_actor->CenterOnCell(a_cell);
            printCell(a_cell);
            logger::debug("teleport interior happened");
            return;
        }
        if (auto worldspace = a_actor->GetWorldspace(); worldspace && worldspace == settings->tamriel_world) {
            int randomCell = RandomInt(settings->teleportCells.size() - 1);
            logger::debug("parent vecor has {} entries", settings->teleportCells.size());
            RE::TESObjectCELL* random_val = settings->teleportCells[randomCell];
            std::jthread([=] {
                std::this_thread::sleep_for(0.7s);
                SKSE::GetTaskInterface()->AddTask([=] {
                    PacifyEnemies(a_cell);
                    });
                }).detach();
            a_actor->CenterOnCell(random_val);
            printCell(random_val);
            logger::debug("teleport exterior happened");
            return;
        }
        else {            
            auto tpcell = findRandomCell(a_actor->GetWorldspace(), a_actor);            
            if (tpcell){
                
                logger::debug("found cell, it is {}", tpcell->GetFormEditorID());
                auto distance = a_actor->GetPlayerRuntimeData().exteriorPosition.GetDistance(RE::NiPoint3(tpcell->GetCoordinates()->worldX, tpcell->GetCoordinates()->worldY, tpcell->GetExteriorWaterHeight()));
                logger::debug("distance to selected cell is {}", distance);
                std::jthread([=] {
                    std::this_thread::sleep_for(0.7s);
                    SKSE::GetTaskInterface()->AddTask([=] {
                        PacifyEnemies(a_cell);
                        });
                    }).detach();
                a_actor->CenterOnCell(tpcell);
            }
            return;                
        }
    }  
    inline static bool ActorHasActiveMagicEffect(RE::Actor* a_actor, RE::EffectSetting* a_effect)
    {

        auto               activeEffects = a_actor->AsMagicTarget()->GetActiveEffectList();
        RE::EffectSetting* setting       = nullptr;
        for (auto& effect : *activeEffects) {
            setting = effect ? effect->GetBaseObject() : nullptr;
            if (setting) {
                if (setting == a_effect) {
                    return true;
                }
            }
        }
        return false;
    }


        static bool IsPermanent(RE::MagicItem* item)
    {
        switch (item->GetSpellType()) {
        case RE::MagicSystem::SpellType::kDisease:
        case RE::MagicSystem::SpellType::kAbility:
        case RE::MagicSystem::SpellType::kAddiction: {
            return true;
        }
        default: {
            return false;
        }
        }
    }

    inline static void ApplySpell(RE::Actor* caster, RE::Actor* target, RE::SpellItem* spell)
    {
        if (IsPermanent(spell)) {
            target->AddSpell(spell);
        }
        else {
            caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->CastSpellImmediate(spell, false, target, 1.0F, false, 0.0F, nullptr);
        }
    }

};
