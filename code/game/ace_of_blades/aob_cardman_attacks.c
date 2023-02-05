#include "ace_of_blades.h"

void initAttacks (mem_arena *memory) {
    aob->allAttacks = cardman_attack_hash_mapInit(memory, 100);

    cardman_attack attack = (cardman_attack){
        .damage = 15.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 60,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "katana_0");

    attack = (cardman_attack){
        .damage = 15.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 30,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "katana_1");

    attack = (cardman_attack){
        .damage = 25.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 20,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "katana_2");

    attack = (cardman_attack){
        .damage = 15.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 25,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "punch_0");

    attack = (cardman_attack){
        .damage = 15.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 28,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "punch_1");

    attack = (cardman_attack){
        .damage = 20.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 35,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "punch_2");

    attack = (cardman_attack){
        .damage = 20.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 60,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "spear_0");

    attack = (cardman_attack){
        .damage = 20.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 47,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "spear_1");

    attack = (cardman_attack){
        .damage = 30.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 43,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "spear_2");

    attack = (cardman_attack){
        .damage = 0.0f,
        .damageMultiplier = 1.0f,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f,
        .linkStartFrame = 45,
    };
    cardman_attack_hash_mapStore(&aob->allAttacks, attack, "wand");
}
