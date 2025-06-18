// ItemProperties.h

#pragma once

#include <unordered_map>
#include <string>
#include <iostream>


class ItemProperties {
public:
    bool is_melee = true;
    std::string name = "";

    // Melee  properties
    int frontal_damage_health = 0;
    int frontal_damage_stamina = 0;
    double frontal_damage_slow = 0;

    int back_damage_health = 0;
    int back_damage_stamina = 0;
    double back_damage_slow = 0;

    int attack_range = 0;
    double attack_speed = 0;
    double cast_time = 0;

    // Gun properties
    int body_damage_health = 0;
    int body_damage_stamina = 0;
    double body_damage_slow = 0;

    int head_damage_health = 0;
    int head_damage_stamina = 0;

    double fire_rate = 0;
    int max_ammo = 0;
    int impact_type = 0;
    int pattern_type = 0;

    // General
    bool auto_fire = true;
    bool can_inventory = true;

    int stamina_cost = 0;

    ItemProperties() = default;

    static ItemProperties CreateMelee(int frontal_health, int frontal_stamina, double frontal_slow,
        int back_health, int back_stamina, double back_slow,
        int range, double speed, int stamina, double cast, bool canInventory) {
        return ItemProperties(true, frontal_health, frontal_stamina, frontal_slow,
            back_health, back_stamina, back_slow, range, speed, stamina, cast, canInventory);
    }

    static ItemProperties CreateGun(int body_health, int body_stamina, double body_slow,
        int head_health, int head_stamina,
        int ammo, double rate, int stamina, int impact, int pattern, bool autoFire, bool canInventory) {
        return ItemProperties(false, body_health, body_stamina, body_slow,
            head_health, head_stamina, ammo, rate, stamina, impact, pattern, autoFire, canInventory);
    }

private:
    ItemProperties(bool isMelee, int frontal_health, int frontal_stamina, double frontal_slow,
        int back_health, int back_stamina, double back_slow,
        int range, double speed, int stamina, double cast, bool canInventory)
        : is_melee(isMelee), frontal_damage_health(frontal_health), frontal_damage_stamina(frontal_stamina),
        frontal_damage_slow(frontal_slow), back_damage_health(back_health), back_damage_stamina(back_stamina),
        back_damage_slow(back_slow), attack_range(range), attack_speed(speed), stamina_cost(stamina),
        cast_time(cast), can_inventory(canInventory) {}

    ItemProperties(bool isMelee, int body_health, int body_stamina, double body_slow,
        int head_health, int head_stamina, int ammo, double rate,
        int stamina, int impact, int pattern, bool autoFire, bool canInventory)
        : is_melee(isMelee), body_damage_health(body_health), body_damage_stamina(body_stamina),
        body_damage_slow(body_slow), head_damage_health(head_health), head_damage_stamina(head_stamina),
        max_ammo(ammo), fire_rate(rate), stamina_cost(stamina), impact_type(impact), pattern_type(pattern),
        auto_fire(autoFire), can_inventory(canInventory) {}
};


extern std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems();
ItemProperties GetItemProperties(const std::string& itemName);

