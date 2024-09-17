#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;
    static void LoadForms() noexcept;

    inline static bool debug_logging{};
    inline static bool always_active{};

    inline static RE::TESObjectCELL* KynesgroveInn;
    inline static RE::TESObjectCELL* WindhelmInn;
    inline static RE::TESObjectCELL* FalkreathInn;
    inline static RE::TESObjectCELL* DragonBridgeInn;
    inline static RE::TESObjectCELL* Solitude;    
    inline static RE::TESObjectCELL* NightgateInn;
    inline static RE::TESObjectCELL* DawnstarInn;
    inline static RE::TESObjectCELL* MarkarthInn;
    inline static RE::TESObjectCELL* HroldanInn;
    inline static RE::TESObjectCELL* RiftenInn;
    inline static RE::TESObjectCELL* IvarsteadInn;
    inline static RE::TESObjectCELL* RoriksteadInn;
    inline static RE::TESObjectCELL* RiverwoodInn;
    inline static RE::TESObjectCELL* WhiterunInn;
    inline static RE::TESObjectCELL* WinterholdInn;
    inline static RE::TESObjectCELL* DHuntsmanInn;    
    inline static RE::TESObjectCELL* SolstheimInn;
    inline static RE::TESObjectCELL* Morthal;
    inline static RE::TESGlobal* inn_price;

    inline static RE::SpellItem* cooldown_spell;
    inline static RE::SpellItem* heal_spell;
    inline static RE::TESObjectARMO* resurrect_ring;
    inline static RE::EffectSetting* cd_effect;



    const std::vector<RE::TESObjectCELL*> teleportCells{
        KynesgroveInn,
        WindhelmInn,
        FalkreathInn,
        DragonBridgeInn,
        Solitude,        
        NightgateInn,
        DawnstarInn,
        MarkarthInn,
        HroldanInn,
        RiftenInn,
        IvarsteadInn,
        RoriksteadInn,
        RiverwoodInn,
        WhiterunInn,
        WinterholdInn,
        DHuntsmanInn,
        Morthal
    };

};
