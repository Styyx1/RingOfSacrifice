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
        }        
        if (auto worldspace = a_actor->GetWorldspace(); worldspace ) {
            if (!worldspace->fullName.contains("Solst")) {
                int randomCell = RandomInt(settings->teleportCells.size());
                logger::debug("parent vecor has {} entries", settings->teleportCells.size());
                RE::TESObjectCELL* random_val = settings->teleportCells[randomCell];
                for (auto& act : GetNearbyActors(a_actor, 1024, false)) {
                    if (act && act->IsInCombat()) {
                        logger::debug("found NPC ref it is: {}", act->GetName());
                        const auto process = RE::ProcessLists::GetSingleton();
                        process->runDetection = false;
                        process->ClearCachedFactionFightReactions();
                        process->StopCombatAndAlarmOnActor(act, false);
                        process->runDetection = true;
                        act->StopCombat();
                    }
                }
                a_actor->CenterOnCell(random_val);
                printCell(random_val);
                logger::debug("teleport exterior happened");
            }
            else {
                RE::TESObjectCELL* random_val = settings->SolstheimInn;
                for (auto& act : GetNearbyActors(a_actor, 1024, false)) {
                    if (act && act->IsInCombat()) {
                        logger::debug("found NPC ref it is: {}", act->GetName());
                        const auto process = RE::ProcessLists::GetSingleton();
                        process->runDetection = false;
                        process->ClearCachedFactionFightReactions();
                        process->StopCombatAndAlarmOnActor(act, false);
                        process->runDetection = true;
                        act->StopCombat();
                    }
                }
                a_actor->CenterOnCell(random_val);
                printCell(random_val);
                logger::debug("teleport solstheim happened");
            }            
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

    inline static void UtilWait(std::chrono::seconds wait_time, RE::PlayerCharacter* player,std::function<void(RE::PlayerCharacter*)> func)
    {
        std::jthread([=] {
            std::this_thread::sleep_for(wait_time);
            SKSE::GetTaskInterface()->AddTask([=] {
                func(player);
                });
            }).detach();
    }

};
