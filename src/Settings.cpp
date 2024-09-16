#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\RingOfSacrifice.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");
    always_active = ini.GetBoolValue("General", "bAlwaysActive");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    // Load settings

    logger::info("Loaded settings");
    logger::info("");
}

void Settings::LoadForms() noexcept
{
    const int id1 = 0x13907;
    const int id2 = 0x16789;
    const int id3 = 0x3A184;
    const int id4 = 0x13A7C;
    const int id5 = 0x16A0E;
    const int id6 = 0x13A5D;
    const int id7 = 0x13A7F;
    const int id8 = 0x16DFE;
    const int id9 = 0x138BE;
    const int id10 = 0x16BDF;
    const int id11 = 0x013A5C;
    const int id12 = 0x13870;
    const int id13 = 0x133C6;
    const int id14 = 0x1605E;
    const int id15 = 0x13814;
    const int id16 = 0x165B7;
    const int id17 = 0x17EC0;
    const int id18 = 0x138CE;
    
    const char* plugin = "Skyrim.esm";
    const char* mod = "RingOfSacrifice.esp";
    const int healID = 0x805;
    const int cooldownID = 0x807;
    const int ring = 0x803;
    const int cdID = 0x806;

    const int inn_priceID = 0x9CC98;
    auto dh = RE::TESDataHandler::GetSingleton();
    inn_price = dh->LookupForm<RE::TESGlobal>(inn_priceID, plugin);
    cooldown_spell = dh->LookupForm<RE::SpellItem>(cooldownID, mod);
    heal_spell = dh->LookupForm<RE::SpellItem>(healID, mod);
    resurrect_ring = dh->LookupForm<RE::TESObjectARMO>(ring, mod);
    cd_effect = dh->LookupForm<RE::EffectSetting>(cdID, mod);
    KynesgroveInn = dh->LookupForm<RE::TESObjectCELL>(id1, plugin);
    WindhelmInn = dh->LookupForm<RE::TESObjectCELL>(id2, plugin);
    FalkreathInn = dh->LookupForm<RE::TESObjectCELL>(id3, plugin);
    DragonBridgeInn = dh->LookupForm<RE::TESObjectCELL>(id4, plugin);
    Solitude = dh->LookupForm<RE::TESObjectCELL>(id5, plugin);    
    NightgateInn = dh->LookupForm<RE::TESObjectCELL>(id6, plugin);
    DawnstarInn = dh->LookupForm<RE::TESObjectCELL>(id7, plugin);
    MarkarthInn = dh->LookupForm<RE::TESObjectCELL>(id8, plugin);
    HroldanInn = dh->LookupForm<RE::TESObjectCELL>(id9, plugin);
    RiftenInn = dh->LookupForm<RE::TESObjectCELL>(id10, plugin);
    IvarsteadInn = dh->LookupForm<RE::TESObjectCELL>(id11, plugin);
    RoriksteadInn = dh->LookupForm<RE::TESObjectCELL>(id12, plugin);
    RiverwoodInn = dh->LookupForm<RE::TESObjectCELL>(id13, plugin);
    WhiterunInn = dh->LookupForm<RE::TESObjectCELL>(id14, plugin);
    WinterholdInn = dh->LookupForm<RE::TESObjectCELL>(id15, plugin);
    DHuntsmanInn = dh->LookupForm<RE::TESObjectCELL>(id16, plugin);    
    SolstheimInn = dh->LookupForm<RE::TESObjectCELL>(id17, "Dragonborn.esm");
    Morthal = dh->LookupForm<RE::TESObjectCELL>(id18, plugin);

    logger::info("loaded forms");
}
