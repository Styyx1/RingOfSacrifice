#include "Utility.h"
#include "Logging.h"
#include "Settings.h"
#include "API/ResurrectionAPI.h"

#undef GetObject

class RingResurrection : public ResurrectionAPI
{
	bool should_resurrect(RE::Actor* a) const override
	{
        Utility* util = Utility::GetSingleton();
        Settings* settings = Settings::GetSingleton();
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

        if (a == player) {
            if (settings->always_active) {
                return settings->always_active;
            }
            return (a->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kRing) == settings->resurrect_ring) && !Utility::ActorHasActiveMagicEffect(a, settings->cd_effect);
        }
        else {
            return (a->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kRing) == settings->resurrect_ring) && !Utility::ActorHasActiveMagicEffect(a, settings->cd_effect);
        }        
	}

	void resurrect(RE::Actor* a) override
	{
        Settings* settings = Settings::GetSingleton();
        RE::TESDataHandler* const dataHandler = RE::TESDataHandler::GetSingleton();
        RE::PlayerCharacter* const player = RE::PlayerCharacter::GetSingleton();
        RE::TESForm* const gold = RE::BGSDefaultObjectManager::GetSingleton()->GetObject(RE::DEFAULT_OBJECT::kGold);
        if (a == player) {
            if (settings->always_active) {
                logger::debug("always active exists");            
                Utility::TeleportPlayer(player);
                Utility::ApplySpell(a,a,settings->heal_spell);                           
                a->RemoveItem(gold->As<RE::TESBoundObject>(), settings->inn_price->value, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
                logger::debug("removed {} septims", settings->inn_price->value);
                return;
            }
            else {
                logger::debug("not always active");
                Utility::ApplySpell(a,a,settings->heal_spell);
                Utility::ApplySpell(a,a,settings->cooldown_spell);
                Utility::TeleportPlayer(player);
                a->RemoveItem(settings->resurrect_ring, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
                a->RemoveItem(gold->As<RE::TESBoundObject>(), settings->inn_price->value, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
                return;
            } 
        }
        else {
            logger::debug("enemy gets resurrected");
            Utility::ApplySpell(a,a,settings->heal_spell);
            Utility::ApplySpell(a,a,settings->cooldown_spell);
            a->RemoveItem(settings->resurrect_ring, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
            a->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, Utility::GetMaxStamina(a));
            return;
        }
               
	}
};

void addSubscriber()
{
	if (auto pluginHandle = REX::W32::GetModuleHandleA("ResurrectionAPI.dll")) {
		if (auto AddSubscriber = (AddSubscriber_t)GetProcAddress(pluginHandle, "ResurrectionAPI_AddSubscriber")) {
			AddSubscriber(std::make_unique<RingResurrection>());
		}
	}
}

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Settings::LoadSettings();
        Settings::LoadForms();
        addSubscriber();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", name, version);

    Init(skse);

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}
