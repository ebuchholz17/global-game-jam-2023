#include "ace_of_blades.h"

aob_state *aob;

#include "aob_cardman_anims.c"
#include "aob_cardman_attacks.c"
#include "aob_powerups.c"

void saveScratchMemPointer (mem_arena *scratchMemory, scratch_mem_save *scratchMemSave) {
    scratchMemSave->current = scratchMemory->current;
}

void restoreScratchMemPointer (mem_arena *scratchMemory, scratch_mem_save *scratchMemSave) {
    scratchMemory->current = scratchMemSave->current;
}

card_suit randomSuit () {
    return randomU32() % CARD_SUIT_JOKER;
}

card_val randomCardVal () {
    return 1 + randomU32() % (CARD_VAL_COUNT - 1);
}

void setAnimStateWithSpeed (cardman *cardman, char *key, f32 speed) {
    cardman->animState.key = key;
    cardman->animState.speedMultiplier = speed;
}

void setAnimState (cardman *cardman, char *key) {
    cardman->animState.key = key;
    cardman->animState.speedMultiplier = 1.0f;

    // hack so i don't have to fix animations
    if (cardman->suit == CARD_SUIT_CLUB && cardman->attack.comboIndex % 3 == 1) {
        cardman->animState.currentFrame = 18;
    }
}

void activateCardMan (cardman_owner owner, vec2 pos, deck_card card) {
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            zeroMemory((u8 *)cm, sizeof(cardman));


            if (owner == CARDMAN_OWNER_PLAYER) {
                aob->playerCardman = cm;
                cm->hitPoints = 100.f;
                cm->maxHitpoints = 100.f;

                *cm = (cardman){
                    .active = true,
                    .owner = owner,

                    .suit = card.suit,
                    .value = card.value,

                    .state = CARDMAN_STATE_IDLE,
                    .facing = DIRECTION_DOWN,
                    .pos = pos,
                    .hitPoints = 100.0f,
                    .maxHitpoints = 100.0f,
                    //.vel
                };

            }
            else {
                *cm = (cardman){
                    .active = true,
                    .owner = owner,

                    .suit = card.suit,
                    .value = card.value,

                    .state = CARDMAN_STATE_IDLE,
                    .facing = DIRECTION_DOWN,
                    .pos = pos,
                    .hitPoints = 40.0f,
                    .maxHitpoints = 40.0f,
                    //.vel
                };
            }

            // set levels based on current difficulty
            i32 difficultyPoints = (i32)aob->difficultyPoints;
            while (difficultyPoints > 0) {
                --difficultyPoints;

                u32 randomUpgrade = randomU32() % 8;
                switch (randomUpgrade) {
                case 0:
                    cm->speedLevel++;
                    break;
                case 1:
                    cm->hitpointLevel++;
                    break;
                case 2:
                    cm->dashLevel++;
                    break;
                case 3:
                    cm->backstepLevel++;
                    break;
                case 4:
                    cm->attackDamageLevel++;
                    break;
                case 5:
                    cm->attackSpeedLevel++;
                    break;
                case 6:
                    cm->attackComboLevel++;
                    break;
                case 7:
                    cm->knockbackLevel++;
                    break;
                }
            }

            cm->hitPoints = getCPUHitpointsForLevel(cm->hitpointLevel);
            cm->maxHitpoints = getCPUHitpointsForLevel(cm->hitpointLevel);


            cm->animState = (animation_state){
                .key = getCardmanIdleAnim(cm),
                .prevKey = getCardmanIdleAnim(cm)
            };


            break;
        }
    }

}

void loadAOBHitboxData (char *key, mem_arena *memory) {
    data_asset *hitboxData = getDataAsset(key);

    char_anim_data *animData = (char_anim_data *)allocMemory(memory, sizeof(char_anim_data));
    animData->key = key;
    loadHitboxData(assetMan, hitboxData->data, animData);

    char_anim_data_ptr_hash_mapStore(&aob->animations, animData, key);
}

deck_card makeMatchingCard (card_suit suit, card_val value) {
    deck_card result;
    if (randomF32() < 0.5f) {
        result.value = value + 1;
        if (result.value == CARD_VAL_COUNT) {
            result.value = CARD_VAL_ACE;
        }
    }
    else {
        result.value = value - 1;
        if (result.value == CARD_VAL_NONE) {
            result.value = CARD_VAL_KING;
        }
    }

    if (suit == CARD_SUIT_SPADE || suit == CARD_SUIT_CLUB) {
        if (randomF32() < 0.5f) {
            result.suit = CARD_SUIT_DIAMOND;
        }
        else {
            result.suit = CARD_SUIT_HEART;
        }
    }
    else {
        if (randomF32() < 0.5f) {
            result.suit = CARD_SUIT_SPADE;
        }
        else {
            result.suit = CARD_SUIT_CLUB;
        }
    }
    return result;
}

deck_card makeJoker () {
    return (deck_card){
        .suit = CARD_SUIT_JOKER,
        .value = CARD_VAL_ACE
    };
}

deck_card makeRandomCard () {
    return (deck_card){
        .suit = randomSuit(),
        .value = randomCardVal()
    };
}

void initAceOfBlades (aob_state* aobState, mem_arena *memory) {
    aob = aobState;
    zeroMemory((u8 *)aob, sizeof(aob_state));

    aob->initialized = true;

    aob->animations = char_anim_data_ptr_hash_mapInit(memory, 200);

    loadAOBHitboxData("cardman_up_idle", memory);
    loadAOBHitboxData("spade_down_idle", memory);
    loadAOBHitboxData("spade_left_idle", memory);
    loadAOBHitboxData("spade_right_idle", memory);
    loadAOBHitboxData("club_down_idle", memory);
    loadAOBHitboxData("club_left_idle", memory);
    loadAOBHitboxData("club_right_idle", memory);
    loadAOBHitboxData("diamond_down_idle", memory);
    loadAOBHitboxData("diamond_left_idle", memory);
    loadAOBHitboxData("diamond_right_idle", memory);
    loadAOBHitboxData("heart_down_idle", memory);
    loadAOBHitboxData("heart_left_idle", memory);
    loadAOBHitboxData("heart_right_idle", memory);

    loadAOBHitboxData("cardman_up_run", memory);
    loadAOBHitboxData("spade_down_run", memory);
    loadAOBHitboxData("spade_left_run", memory);
    loadAOBHitboxData("spade_right_run", memory);
    loadAOBHitboxData("club_down_run", memory);
    loadAOBHitboxData("club_left_run", memory);
    loadAOBHitboxData("club_right_run", memory);
    loadAOBHitboxData("diamond_down_run", memory);
    loadAOBHitboxData("diamond_left_run", memory);
    loadAOBHitboxData("diamond_right_run", memory);
    loadAOBHitboxData("heart_down_run", memory);
    loadAOBHitboxData("heart_left_run", memory);
    loadAOBHitboxData("heart_right_run", memory);

    loadAOBHitboxData("cardman_up_katana_0", memory);
    loadAOBHitboxData("spade_down_katana_0", memory);
    loadAOBHitboxData("spade_left_katana_0", memory);
    loadAOBHitboxData("spade_right_katana_0", memory);

    loadAOBHitboxData("spade_left_hitstun", memory);
    loadAOBHitboxData("spade_right_hitstun", memory);
    loadAOBHitboxData("club_left_hitstun", memory);
    loadAOBHitboxData("club_right_hitstun", memory);
    loadAOBHitboxData("diamond_left_hitstun", memory);
    loadAOBHitboxData("diamond_right_hitstun", memory);
    loadAOBHitboxData("heart_left_hitstun", memory);
    loadAOBHitboxData("heart_right_hitstun", memory);
    loadAOBHitboxData("joker_left_hitstun", memory);
    loadAOBHitboxData("joker_right_hitstun", memory);

    loadAOBHitboxData("spade_play_card_0", memory);
    loadAOBHitboxData("spade_play_card_1", memory);
    loadAOBHitboxData("club_play_card_0", memory);
    loadAOBHitboxData("club_play_card_1", memory);
    loadAOBHitboxData("diamond_play_card_0", memory);
    loadAOBHitboxData("diamond_play_card_1", memory);
    loadAOBHitboxData("heart_play_card_0", memory);
    loadAOBHitboxData("heart_play_card_1", memory);

    loadAOBHitboxData("spade_left_dash", memory);
    loadAOBHitboxData("spade_right_dash", memory);
    loadAOBHitboxData("spade_down_dash", memory);
    loadAOBHitboxData("spade_left_backstep", memory);
    loadAOBHitboxData("spade_right_backstep", memory);
    loadAOBHitboxData("spade_down_backstep", memory);

    loadAOBHitboxData("club_left_dash", memory);
    loadAOBHitboxData("club_right_dash", memory);
    loadAOBHitboxData("club_down_dash", memory);
    loadAOBHitboxData("club_left_backstep", memory);
    loadAOBHitboxData("club_right_backstep", memory);
    loadAOBHitboxData("club_down_backstep", memory);

    loadAOBHitboxData("diamond_left_dash", memory);
    loadAOBHitboxData("diamond_right_dash", memory);
    loadAOBHitboxData("diamond_down_dash", memory);
    loadAOBHitboxData("diamond_left_backstep", memory);
    loadAOBHitboxData("diamond_right_backstep", memory);
    loadAOBHitboxData("diamond_down_backstep", memory);

    loadAOBHitboxData("heart_left_dash", memory);
    loadAOBHitboxData("heart_right_dash", memory);
    loadAOBHitboxData("heart_down_dash", memory);
    loadAOBHitboxData("heart_left_backstep", memory);
    loadAOBHitboxData("heart_right_backstep", memory);
    loadAOBHitboxData("heart_down_backstep", memory);

    loadAOBHitboxData("cardman_up_dash", memory);
    loadAOBHitboxData("cardman_up_backstep", memory);

    loadAOBHitboxData("cardman_up_katana_1", memory);
    loadAOBHitboxData("cardman_up_katana_2", memory);
    loadAOBHitboxData("cardman_up_punch_0", memory);
    loadAOBHitboxData("cardman_up_punch_1", memory);
    loadAOBHitboxData("cardman_up_spear_0", memory);
    loadAOBHitboxData("cardman_up_spear_1", memory);
    loadAOBHitboxData("cardman_up_wand", memory);
    loadAOBHitboxData("club_down_punch_0", memory);
    loadAOBHitboxData("club_down_punch_1", memory);
    loadAOBHitboxData("club_left_punch_0", memory);
    loadAOBHitboxData("club_left_punch_1", memory);
    loadAOBHitboxData("club_right_punch_0", memory);
    loadAOBHitboxData("club_right_punch_1", memory);
    loadAOBHitboxData("diamond_down_wand", memory);
    loadAOBHitboxData("diamond_left_wand", memory);
    loadAOBHitboxData("diamond_right_wand", memory);
    loadAOBHitboxData("heart_down_spear_0", memory);
    loadAOBHitboxData("heart_down_spear_1", memory);
    loadAOBHitboxData("heart_left_spear_0", memory);
    loadAOBHitboxData("heart_left_spear_1", memory);
    loadAOBHitboxData("heart_right_spear_0", memory);
    loadAOBHitboxData("heart_right_spear_1", memory);
    loadAOBHitboxData("heart_spear_spin", memory);
    loadAOBHitboxData("spade_down_katana_1", memory);
    loadAOBHitboxData("spade_down_katana_2", memory);
    loadAOBHitboxData("spade_left_katana_1", memory);
    loadAOBHitboxData("spade_left_katana_2", memory);
    loadAOBHitboxData("spade_right_katana_1", memory);
    loadAOBHitboxData("spade_right_katana_2", memory);
    loadAOBHitboxData("star", memory);

    loadAOBHitboxData("joker_down_idle", memory);
    loadAOBHitboxData("joker_down_run", memory);
    loadAOBHitboxData("joker_left_idle", memory);
    loadAOBHitboxData("joker_left_run", memory);
    loadAOBHitboxData("joker_right_idle", memory);
    loadAOBHitboxData("joker_right_run", memory);
    loadAOBHitboxData("joker_play_card_0", memory);
    loadAOBHitboxData("joker_play_card_1", memory);

    initAttacks(memory);

}

void startGame (mem_arena *memory) {
    aob->gameStarted = true;
    zeroMemory((u8 *)&aob->cardmen, sizeof(cardman) * MAX_NUM_CARDMEN);
    zeroMemory((u8 *)&aob->decks, sizeof(deck) * NUM_DECKS);
    zeroMemory((u8 *)&aob->bullets, sizeof(bullet_star) * MAX_NUM_BULLETS);
    aob->spawnTimer = 0.0f;
    aob->difficultyTimer = 0.0f;
    aob->difficultyPoints = 0;
    aob->playedCardInfo.active = false;
    aob->lastHitEnemy = 0;
    aob->guideOpen = false;
    aob->newUpgradeTimer = 0.0f;
    aob->jokerTimer = 0.0f;

    vec2 playerPos = (vec2){ .x = 240.0f, .y = 120.0f};
    deck_card playerCard = makeRandomCard();
    activateCardMan(CARDMAN_OWNER_PLAYER, playerPos, playerCard);

    aob->upgrades = player_upgrade_listInit(memory, 300);

    for (u32 i = 0; i < 2; i++) {
        f32 t = randomF32();
        vec2 pos = vec2Add(playerPos, (vec2){ 
            .x = cos2PI(t) * 280.0f + 100.0f * randomF32(), 
            .y = sin2PI(t) * 280.0f + 100.0f * randomF32() 
        });
        activateCardMan(CARDMAN_OWNER_CPU, pos, makeRandomCard());
    }

    for (u32 i = 0; i < NUM_DECKS; i++) {
        deck *d = &aob->decks[i];
        // oops, probably will run out of memory after a long time
        d->cards = deck_card_listInit(memory, STARTING_CARDS_PER_DECK * 2);

        for (u32 j = 0; j < STARTING_CARDS_PER_DECK; j++) {
            deck_card_listPush(&d->cards, (deck_card){
                .suit = randomSuit(),
                .value = randomCardVal()
            });
        }
        d->pos.x = -35.0f + (i % 2) * 70.0f;
        d->pos.y = -35.0f + (i / 2) * 70.0f;
    }
}


aob_input parseGameInput (game_input *input, virtual_input *vInput) {
    aob_input result = {0};
    if (input->leftArrow.down || 
        input->rightArrow.down || 
        input->upArrow.down || 
        input->downArrow.down ||
        input->aKey.down ||
        input->zKey.down ||
        input->sKey.down ||
        input->xKey.down) 
    {
        aob->inputSource = INPUT_SOURCE_KEYBOARD;
    }
    else if (vInput->dPadUp.button.down || 
             vInput->dPadDown.button.down || 
             vInput->dPadLeft.button.down || 
             vInput->dPadRight.button.down ||
             vInput->topButton.button.down || 
             vInput->bottomButton.button.down || 
             vInput->leftButton.button.down || 
             vInput->rightButton.button.down)
    {
        aob->inputSource = INPUT_SOURCE_VIRTUAL;
    }
    else {
        for (u32 controllerIndex = 0; controllerIndex < MAX_NUM_CONTROLLERS; controllerIndex++) {
            game_controller_input *cont = &input->controllers[controllerIndex];

            if (cont->connected) {
                b32 useController;
                if (cont->dPadUp.down || cont->dPadLeft.down || cont->dPadDown.down || cont->dPadRight.down ||
                    cont->aButton.down || cont->bButton.down || cont->xButton.down || cont->yButton.down) 
                {
                    aob->inputSource = INPUT_SOURCE_GAMEPAD;
                    break;
                }
            }
        }
    }

    switch (aob->inputSource) {
        case INPUT_SOURCE_KEYBOARD: {
            result.left = input->leftArrow;
            result.right = input->rightArrow;
            result.up = input->upArrow;
            result.down = input->downArrow;
            result.attack = input->zKey;
            result.dash = input->xKey;
            result.playCard = input->aKey;
            result.pause = input->sKey;
        } break;
        case INPUT_SOURCE_VIRTUAL: {
            result.up = vInput->dPadUp.button;;
            result.down = vInput->dPadDown.button;
            result.left = vInput->dPadLeft.button;
            result.right = vInput->dPadRight.button;
            result.attack = vInput->bottomButton.button;
            result.dash = vInput->leftButton.button;
            result.playCard = vInput->rightButton.button;
            //result.pause = vInput->topButton.button;
        } break;
        case INPUT_SOURCE_GAMEPAD: {
            for (u32 controllerIndex = 0; controllerIndex < MAX_NUM_CONTROLLERS; controllerIndex++) {
                game_controller_input *cont = &input->controllers[controllerIndex];

                if (cont->connected) {
                    result.left = cont->dPadLeft;
                    result.right = cont->dPadRight;
                    result.up = cont->dPadUp;
                    result.down = cont->dPadDown;
                    result.attack = cont->aButton;
                    result.dash = cont->xButton;
                    result.playCard = cont->bButton;
                    result.pause = cont->yButton;
                    break;
                }
            }
        } break;
    }

    return result;
}

void activateBullet (vec2 pos, vec2 dir, f32 damage, cardman_owner owner, f32 damageMultiplier, f32 speedMultiplier) {
    bullet_star *bullet;
    for (u32 i = 0; i < MAX_NUM_BULLETS; i++) {
        bullet_star *b = &aob->bullets[i];
        if (!b->active) {
            bullet = b;
            break;
        }
    }

    if (bullet != 0) {
        *bullet = (bullet_star){
            .active = true,
            .pos = vec2Add(pos, (vec2){ .x = 0.0f, .y = -60.0f }),
            .vel = vec2ScalarMul(BULLET_SPEED, dir),
            .damage = damage * damageMultiplier,
            .owner = owner,
            .attackID = randomU32(),
            .animState = (animation_state){
                .key = "star",
                .speedMultiplier = speedMultiplier
            }
        };
    }
}

void spawnCardmanBullets (cardman *cm) {
    // determine properties
    vec2 dir = (vec2){0};
    if (cm->facing == DIRECTION_RIGHT) {
        dir = (vec2){ .x = 1.0f, .y = 0.0f};
    }
    else if (cm->facing == DIRECTION_LEFT) {
        dir = (vec2){ .x = -1.0f, .y = 0.0f};
    }
    else if (cm->facing == DIRECTION_UP) {
        dir = (vec2){ .x = 0.0f, .y = -1.0f};
    }
    else if (cm->facing == DIRECTION_DOWN) {
        dir = (vec2){ .x = 0.0f, .y = 1.0f};
    }

    f32 damage = getAttackDamageMultiplierForLevel(cm->attackDamageLevel);
    f32 speed = getAttackSpeedMultiplierForLevel(cm->attackSpeedLevel);

    f32 baseDamage = 9.0f;

    // spawn bullets
    u32 katanaComb = cm->attack.comboIndex % 3;
    if (katanaComb == 0) {
        activateBullet(cm->pos, dir, baseDamage, cm->owner, damage, speed);
    }
    else if (katanaComb == 1) {
        if (cm->facing == DIRECTION_RIGHT) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = -0.25f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.25f })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_LEFT) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = -0.25f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.25f })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_UP) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = -0.25f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.25f, .y = 0.0 })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_DOWN) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = -0.25f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.25f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
        }
    }
    else if (katanaComb == 2) {
        if (cm->facing == DIRECTION_RIGHT) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = -0.5f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.5f })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_LEFT) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = -0.5f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.5f })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_UP) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = -0.5f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.5f, .y = 0.0 })), baseDamage, cm->owner, damage, speed);
        }
        else if (cm->facing == DIRECTION_DOWN) {
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = -0.5f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.0f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
            activateBullet(cm->pos, vec2Normalize(vec2Add(dir, (vec2){ .x = 0.5f, .y = 0.0f })), baseDamage, cm->owner, damage, speed);
        }
    }
    soundManPlaySound("starSFX");
}

void setCardmanAttackInfo (cardman *cm) {
    u32 comboIndex = 0;

    if (cm->attack.active) {
        comboIndex = cm->attack.comboIndex + 1;
    }

    switch (cm->suit) {
        case CARD_SUIT_SPADE: {
            u32 katanaComb = comboIndex % 3;
            switch (katanaComb) {
            case 0: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "katana_0");
                break;
            case 1: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "katana_1");
                break;
            case 2: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "katana_2");
                break;
            }

            soundManPlaySound("attack");
            cm->attack.animKey = getCardmanKatanaAnim(cm, katanaComb);
        } break;
        case CARD_SUIT_CLUB: {
            u32 katanaComb = comboIndex % 3;
            switch (katanaComb) {
            case 0: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "punch_0");
                break;
            case 1: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "punch_1");
                break;
            case 2: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "punch_2");
                break;
            }

            soundManPlaySound("attack");
            cm->attack.animKey = getCardmanPunchAnim(cm, katanaComb);
        } break;
        case CARD_SUIT_DIAMOND: {
            cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "wand");

            cm->attack.animKey = getCardmanWandAnim(cm);
        } break;
        case CARD_SUIT_HEART: {
            u32 katanaComb = comboIndex % 3;
            switch (katanaComb) {
            case 0: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "spear_0");
                break;
            case 1: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "spear_1");
                break;
            case 2: 
                cm->attack = cardman_attack_hash_mapGetVal(&aob->allAttacks, "spear_2");
                break;
            }

            soundManPlaySound("attack");
            cm->attack.animKey = getCardmanSpearAnim(cm, katanaComb);
        } break;
    }
    cm->attack.comboIndex = comboIndex;
    cm->attack.active = true;
    cm->attack.id = randomU32();
    cm->attack.damageMultiplier = getAttackDamageMultiplierForLevel(cm->attackDamageLevel);
    cm->attack.speedMultiplier = getAttackSpeedMultiplierForLevel(cm->attackSpeedLevel);
}

b32 cardsCanMatch (card_suit firstSuit, card_val firstValue, card_suit secondSuit, card_val secondValue) {
    if (firstSuit == CARD_SUIT_JOKER || secondSuit == CARD_SUIT_JOKER) {
        return true;
    }

    b32 firstSuitIsBlack = firstSuit == CARD_SUIT_SPADE || firstSuit == CARD_SUIT_CLUB;
    b32 secondSuitIsBlack = secondSuit == CARD_SUIT_SPADE || secondSuit == CARD_SUIT_CLUB;

    b32 suitsMatch = firstSuitIsBlack != secondSuitIsBlack;

    b32 valsMatch;
    if ((firstValue == CARD_VAL_ACE && secondValue == CARD_VAL_KING) ||
        (secondValue == CARD_VAL_ACE && firstValue == CARD_VAL_KING))
    {
        valsMatch = true;
    }
    else {
        i32 valDiff = (i32)firstValue - (i32)secondValue;
        if (valDiff < 0) { valDiff = -valDiff; }
        valsMatch = valDiff == 1;
    }

    return suitsMatch && valsMatch;
}

void cardmenBubbleSortDistance (cardman_ptr_list *cardmen) {
    cardman *player = aob->playerCardman;
    for (int i = 0; i < cardmen->numValues; ++i) {
        for (int j = i; j > 0; --j) {
            cardman *first = cardmen->values[j-1];
            cardman *second = cardmen->values[j];

            vec2 firstToPlayer = vec2Subtract(player->pos, first->pos);
            f32 firstDistToPlayer = vec2Length(firstToPlayer);
            vec2 secondToPlayer = vec2Subtract(player->pos, second->pos);
            f32 secondDistToPlayer = vec2Length(secondToPlayer);

            if (secondDistToPlayer < firstDistToPlayer) {
                cardmen->values[j-1] = second;
                cardmen->values[j] = first;
            }
        }
    }
}

void tryPlayCard (cardman *cm, mem_arena *scratchMemory) {

    saveScratchMemPointer(scratchMemory, &aob->scratchMemSave);

    cardman_ptr_list cardmen = cardman_ptr_listInit(scratchMemory, MAX_NUM_CARDMEN);
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        // TODO: draw player characters on top?
        cardman *enemy = &aob->cardmen[cardmanIndex];

        if (enemy->active && enemy != cm && enemy->defeatedTimer > 0.0f) {
            cardman_ptr_listPush(&cardmen, enemy);
        }
    }

    cardmenBubbleSortDistance(&cardmen);

    b32 playedCardFromEnemy = false;
    for (u32 cardmanIndex = 0; cardmanIndex < cardmen.numValues; cardmanIndex++) {
        cardman *enemy = cardmen.values[cardmanIndex];

        if (cardsCanMatch(cm->suit, cm->value, enemy->suit, enemy->value)) {
            vec2 playerToEnemy = vec2Subtract(cm->pos, enemy->pos);
            f32 distToEnemy = vec2Length(playerToEnemy);

            if (distToEnemy <= DIST_TO_PLAY_CARD) {
                cm->state = CARDMAN_STATE_PLAYING_CARD;
                setAnimState(cm, getCardmanPlayCardStartAnim(cm));

                aob->playedCardInfo = (played_card_info){
                    .active = true,
                    .suit = enemy->suit,
                    .value = enemy->value,
                    .cardStartPos = enemy->pos,
                    .phase = PLAYED_CARD_PHASE_MOVE_CARD_ABOVE,
                    .t = 0.0f,
                };

                soundManPlaySound("card_flip");
                playedCardFromEnemy = true;
                enemy->active = false;

                break;
            }
        }
    }
    restoreScratchMemPointer(scratchMemory, &aob->scratchMemSave);

    if (!playedCardFromEnemy) {
        for (u32 deckIndex = 0; deckIndex < NUM_DECKS; deckIndex++) {
            deck *d = &aob->decks[deckIndex];

            if (d->cards.numValues <= 0) {
                continue;
            }

            deck_card topCard = d->cards.values[d->cards.numValues - 1];
            if (cardsCanMatch(cm->suit, cm->value, topCard.suit, topCard.value)) {

                vec2 playerToDeck = vec2Subtract(vec2Add(d->pos,(vec2){ .x = 16.0f, .y = 39.0f }), cm->pos);
                f32 distToDeck = vec2Length(playerToDeck);

                if (distToDeck <= DIST_TO_PLAY_CARD_TO_DECK) {
                    cm->state = CARDMAN_STATE_PLAYING_CARD;
                    setAnimState(cm, getCardmanPlayCardStartAnim(cm));

                    aob->playedCardInfo = (played_card_info){
                        .active = true,
                        .suit = topCard.suit,
                        .value = topCard.value,
                        .cardStartPos = vec2Add(d->pos, (vec2){ .x = 16.0f, .y = 39.0f}),
                        .phase = PLAYED_CARD_PHASE_MOVE_CARD_ABOVE,
                        .t = 0.0f,
                    };

                    soundManPlaySound("card_flip");
                    deck_card_listSplice(&d->cards, d->cards.numValues - 1);
                    break;

                    //// TODO: remove card from player
                    //if (aob->upgrades.numValues == 0) {
                    //    cm->active = false;
                    //}
                    //else {
                    //    player_upgrade pu = aob->upgrades.values[aob->upgrades.numValues - 1];
                    //    player_upgrade_listSplice(&aob->upgrades, aob->upgrades.numValues - 1);

                    //    powerup_type powerup = getUpgradeTypeForSuitValue(cm->suit, cm->value);
                    //    switch (powerup) {
                    //        case POWERUP_TYPE_SPEED: {
                    //            cm->speedLevel--;
                    //            if (cm->speedLevel < 0) {
                    //                cm->speedLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_HITPOINTS: {
                    //            cm->hitpointLevel--;
                    //            if (cm->hitpointLevel < 0) {
                    //                cm->hitpointLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_DASH: {
                    //            cm->dashLevel--;
                    //            if (cm->dashLevel < 0) {
                    //                cm->dashLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_BACKSTEP: {
                    //            cm->backstepLevel--;
                    //            if (cm->backstepLevel < 0) {
                    //                cm->backstepLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_ATTACK_DAMAGE: {
                    //            cm->attackDamageLevel--;
                    //            if (cm->attackDamageLevel < 0) {
                    //                cm->attackDamageLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_ATTACK_SPEED: {
                    //            cm->attackSpeedLevel--;
                    //            if (cm->attackSpeedLevel < 0) {
                    //                cm->attackSpeedLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_ATTACK_COMBO: {
                    //            cm->attackComboLevel--;
                    //            if (cm->attackComboLevel < 0) {
                    //                cm->attackComboLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_KNOCKBACK: {
                    //            cm->knockbackLevel--;
                    //            if (cm->knockbackLevel < 0) {
                    //                cm->knockbackLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_INCREASE_ENEMY_HITSTUN: {
                    //            cm->enemyHitstunLevel--;
                    //            if (cm->enemyHitstunLevel < 0) {
                    //                cm->enemyHitstunLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_DECREASE_PLAYER_HITSTUN: {
                    //            cm->playerHitstunLevel--;
                    //            if (cm->playerHitstunLevel < 0) {
                    //                cm->playerHitstunLevel = 0;
                    //            }
                    //        } break;
                    //        case POWERUP_TYPE_INVINCIBILITY_FRAMES: {
                    //            cm->invincibilityFramesLevel--;
                    //            if (cm->invincibilityFramesLevel < 0) {
                    //                cm->invincibilityFramesLevel = 0;
                    //            }
                    //        } break;
                    //    }
                    //    cm->maxHitpoints = getHitpointsForLevel(cm->hitpointLevel);

                    //    cm->suit = pu.suit;
                    //    cm->value = pu.value;
                    //    cm->hitPoints = cm->maxHitpoints;
                    //    setAnimState(cm, getCardmanPlayCardEndAnim(cm));
                    //}

                    break;
                }
            }
        }
    }
}

b32 testCardmanAttackHitCardman (cardman *enemy, char_frame_data *attackerFrame, mat3x3 *attackerTransform) 
{
    animation_state *enemyAnimState = &enemy->animState;
    char_anim_data *enemyAnimData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, enemyAnimState->key);
    char_frame_data *enemyCurrentFrame = &enemyAnimData->frames[enemyAnimState->currentFrame];

    mat3x3 enemyTransform = mat3x3Translate(enemy->pos.x, enemy->pos.y);

    b32 attackerHitEnemy = false;
    for (int hitboxIndex = 0; hitboxIndex < attackerFrame->numHitboxes; ++hitboxIndex) {
        rect hitbox = attackerFrame->hitboxes[hitboxIndex];
        vec2 hitMin = vec2Mat3x3Mul(*attackerTransform, hitbox.min);
        vec2 hitMax = vec2Mat3x3Mul(*attackerTransform, hitbox.max);
        rect transformedHitbox = (rect){ .min = hitMin, .max = hitMax };

        for (int hurtboxIndex = 0; hurtboxIndex < enemyCurrentFrame->numHurtboxes; ++hurtboxIndex) {
            rect hurtbox = enemyCurrentFrame->hurtboxes[hurtboxIndex];

            vec2 hurtMin = vec2Mat3x3Mul(enemyTransform, hurtbox.min);
            vec2 hurtMax = vec2Mat3x3Mul(enemyTransform, hurtbox.max);
            rect transformedHurtbox = (rect){ .min = hurtMin, .max = hurtMax };

            if (rectsIntersect(transformedHitbox, transformedHurtbox)) {
                return true;
            }
        }
    }
    return false;
}

void updateAceOfBlades (game_input *input, virtual_input *vInput, f32 dt, 
                        mem_arena *memory, mem_arena *scratchMemory, plat_api platAPI) 
{
    aob_input aobInput = parseGameInput(input, vInput);

    if (aob->phase == AOB_PHASE_TITLE) {
        if (aobInput.up.justPressed || aobInput.down.justPressed || aobInput.left.justPressed || aobInput.right.justPressed ||
            aobInput.attack.justPressed || aobInput.dash.justPressed || aobInput.playCard.justPressed || aobInput.pause.justPressed)
        {
            aob->gameStarted = false;
            aob->phase = AOB_PHASE_GAMEPLAY;
        }
        return;
    }
    else if (aob->phase == AOB_PHASE_GAMEOVER) {
        if (aobInput.up.justPressed || aobInput.down.justPressed || aobInput.left.justPressed || aobInput.right.justPressed ||
            aobInput.attack.justPressed || aobInput.dash.justPressed || aobInput.playCard.justPressed || aobInput.pause.justPressed)
        {
            aob->phase = AOB_PHASE_TITLE;
        }
        return;
    }
    else if (aob->phase == AOB_PHASE_WIN) {
        if (aobInput.up.justPressed || aobInput.down.justPressed || aobInput.left.justPressed || aobInput.right.justPressed ||
            aobInput.attack.justPressed || aobInput.dash.justPressed || aobInput.playCard.justPressed || aobInput.pause.justPressed)
        {
            aob->phase = AOB_PHASE_TITLE;
        }
        return;
    }

    if (!aob->gameStarted) {
        startGame(memory);
    }

    if (aobInput.pause.justPressed) {
        aob->guideOpen = !aob->guideOpen;
    }

    if (aob->guideOpen) {
        return;
    }
    aob->gameTime += dt;


    b32 allEmpty = true;
    for (u32 deckIndex = 0; deckIndex < NUM_DECKS; ++deckIndex) {
        deck *d = &aob->decks[deckIndex];
        if (d->cards.numValues > 0) {
            allEmpty = false;
            break;
        }
    }
    if (allEmpty && aob->playerCardman->state == CARDMAN_STATE_IDLE) {
        aob->phase = AOB_PHASE_WIN;
        return;
    }

    aob->spawnTimer += dt;
    while (aob->spawnTimer > 3.8f) {
        aob->spawnTimer -= 3.8f;
        f32 matchingChance = 1.0f / 3.25f;
        f32 jokerChance = 1.0f/40.0f;
        deck_card card;
        f32 roll = randomF32();
        if (roll < jokerChance) {
            card = makeJoker();
        }
        else if (roll < matchingChance) {
            card = makeMatchingCard(aob->playerCardman->suit, aob->playerCardman->value);
        }
        else {
            card = makeRandomCard();
        }

        f32 t = randomF32();
        vec2 pos = vec2Add(aob->playerCardman->pos, (vec2){ 
            .x = cos2PI(t) * 500.0f + 100.0f * randomF32(), 
            .y = sin2PI(t) * 500.0f + 100.0f * randomF32() 
        });
        activateCardMan(CARDMAN_OWNER_CPU, pos, card);
    }

    aob->difficultyTimer += dt;
    while (aob->difficultyTimer > 50.0f) {
        aob->difficultyTimer -= 50.0f;
        aob->difficultyPoints++;
    }

    if (aob->jokerTimer > 0.0f) {
        aob->jokerTimer -= dt;
        if (aob->jokerTimer < 0.0f) {
            player_upgrade pu = aob->upgrades.values[aob->upgrades.numValues - 1];
            player_upgrade_listSplice(&aob->upgrades, aob->upgrades.numValues - 1);

            aob->playerCardman->suit = pu.suit;
            aob->playerCardman->value = pu.value;
            //setAnimState(aob->playerCardman, get(cm));
        }
    }

    // update controls/movement
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        zeroMemory((u8 *)&cm->hitByInfo, sizeof(cardman_hitby_info));

        if (cm->state != CARDMAN_STATE_ATTACKING) {
            zeroMemory((u8 *)&cm->attack, sizeof(cardman_attack));
        }

        cm->animState.prevKey = cm->animState.key;
        cm->lastPosCounter++;
        if (cm->lastPosCounter > 20) {
            cm->lastPosCounter = 0;
            cm->lastPositions[cm->lastPosIndex] = cm->pos;
            cm->lastPosIndex = (cm->lastPosIndex + 1) % 3;
        }

        if (cm->owner == CARDMAN_OWNER_PLAYER) {
            switch (cm->state) {
                case CARDMAN_STATE_IDLE: {
                    cm->vel = (vec2){
                        .x = 0.0f,
                        .y = 0.0f
                    };
                    setAnimState(cm, getCardmanIdleAnim(cm));
                    if (aobInput.up.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_UP;
                        setAnimState(cm, getCardmanWalkingAnim(cm));
                    }
                    if (aobInput.down.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_DOWN;
                        setAnimState(cm, getCardmanWalkingAnim(cm));
                    }
                    if (aobInput.left.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_LEFT;
                        setAnimState(cm, getCardmanWalkingAnim(cm));
                    }
                    if (aobInput.right.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_RIGHT;
                        setAnimState(cm, getCardmanWalkingAnim(cm));
                    }

                    if (cm->suit != CARD_SUIT_JOKER) {
                        if (aobInput.attack.justPressed) {
                            cm->state = CARDMAN_STATE_ATTACKING;
                            setCardmanAttackInfo(cm);
                            setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                        }
                        if (aobInput.dash.justPressed && cm->backstepLevel > 0) {
                            vec2 moveDir = {0};
                            if (cm->facing == DIRECTION_UP) {
                                moveDir.y = -1.0f;
                            }
                            if (cm->facing == DIRECTION_DOWN) {
                                moveDir.y = 1.0f;
                            }
                            if (cm->facing == DIRECTION_LEFT) {
                                moveDir.x = -1.0f;
                            }
                            if (cm->facing == DIRECTION_RIGHT) {
                                moveDir.x = 1.0f;
                            }

                            soundManPlaySound("dash");
                            cm->state = CARDMAN_STATE_DASHING;
                            cm->dashVel = vec2ScalarMul(-1.0f * getBackstepSpeedForLevel(cm->backstepLevel), moveDir);
                            cm->dodgedAttackDuringThisDodge = false;
                            cm->dashTimer = 0.3f;
                            setAnimState(cm, getCardmanBackstepAnim(cm));
                        }
                    }
                    if (aobInput.playCard.justPressed) {
                        tryPlayCard(cm, scratchMemory);
                    }
                } break;
                case CARDMAN_STATE_WALKING: {
                    if (!aobInput.up.down &&
                        !aobInput.down.down &&
                        !aobInput.left.down &&
                        !aobInput.right.down)
                    {
                        cm->state = CARDMAN_STATE_IDLE;
                        setAnimState(cm, getCardmanIdleAnim(cm));
                    }
                    else {
                        vec2 moveDir = {0};
                        if (aobInput.up.down) {
                            moveDir.y = -1.0f;
                        }
                        if (aobInput.down.down) {
                            moveDir.y = 1.0f;
                        }
                        if (aobInput.left.down) {
                            moveDir.x = -1.0f;
                        }
                        if (aobInput.right.down) {
                            moveDir.x = 1.0f;
                        }

                        moveDir = vec2Normalize(moveDir);
                        cm->vel = vec2ScalarMul(getMoveSpeedForLevel(cm->speedLevel), moveDir);

                        cm->facing = getCardmanFacingForMoveDir(cm, moveDir);
                        setAnimStateWithSpeed(cm, getCardmanWalkingAnim(cm), getMoveSpeedForLevel(cm->speedLevel) / 150.0f);

                        if (cm->suit != CARD_SUIT_JOKER) {
                            if (aobInput.dash.justPressed && cm->dashLevel > 0) {
                                soundManPlaySound("dash");
                                cm->state = CARDMAN_STATE_DASHING;
                                cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                cm->dodgedAttackDuringThisDodge = false;
                                cm->dashTimer = 0.3f;
                                setAnimState(cm, getCardmanDashingAnim(cm));
                            }
                        }
                    }

                    if (cm->suit != CARD_SUIT_JOKER) {
                        if (aobInput.attack.justPressed) {
                            cm->state = CARDMAN_STATE_ATTACKING;
                            setCardmanAttackInfo(cm);
                            setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                            startAnimState(&cm->animState);
                        }
                    }
                    if (aobInput.playCard.justPressed) {
                        tryPlayCard(cm, scratchMemory);
                    }
                } break;
                case CARDMAN_STATE_ATTACKING: {
                    // TODO: linked attack/dash
                    if (cm->suit == CARD_SUIT_DIAMOND && cm->animState.totalFrames >= 40.0f && !cm->attack.spawnedBullets) {
                        cm->attack.spawnedBullets = true;
                        spawnCardmanBullets(cm);
                    }

                    if (cm->animState.totalFrames >= cm->attack.linkStartFrame) {
                        if (aobInput.attack.justPressed && cm->attackComboLevel > cm->attack.comboIndex) {

                            if (aobInput.up.down) {
                                cm->facing = DIRECTION_UP;
                            }
                            if (aobInput.down.down) {
                                cm->facing = DIRECTION_DOWN;
                            }
                            if (aobInput.left.down) {
                                cm->facing = DIRECTION_LEFT;
                            }
                            if (aobInput.right.down) {
                                cm->facing = DIRECTION_RIGHT;
                            }

                            cm->state = CARDMAN_STATE_ATTACKING;
                            setCardmanAttackInfo(cm);
                            setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                            startAnimState(&cm->animState);
                        }

                        if (aobInput.dash.justPressed) {
                            b32 dash = false;
                            if (aobInput.up.down) {
                                cm->facing = DIRECTION_UP;
                                dash = true;
                            }
                            if (aobInput.down.down) {
                                cm->facing = DIRECTION_DOWN;
                                dash = true;
                            }
                            if (aobInput.left.down) {
                                cm->facing = DIRECTION_LEFT;
                                dash = true;
                            }
                            if (aobInput.right.down) {
                                cm->facing = DIRECTION_RIGHT;
                                dash = true;
                            }

                            if (dash && cm->dashLevel > 0) {
                                vec2 moveDir = {0};
                                if (cm->facing == DIRECTION_UP) {
                                    moveDir.y = -1.0f;
                                }
                                if (cm->facing == DIRECTION_DOWN) {
                                    moveDir.y = 1.0f;
                                }
                                if (cm->facing == DIRECTION_LEFT) {
                                    moveDir.x = -1.0f;
                                }
                                if (cm->facing == DIRECTION_RIGHT) {
                                    moveDir.x = 1.0f;
                                }

                                soundManPlaySound("dash");
                                cm->state = CARDMAN_STATE_DASHING;
                                cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                cm->dodgedAttackDuringThisDodge = false;
                                cm->dashTimer = 0.3f;
                                setAnimState(cm, getCardmanBackstepAnim(cm));
                            }
                            else if (cm->backstepLevel > 0) {
                                vec2 moveDir = {0};
                                if (cm->facing == DIRECTION_UP) {
                                    moveDir.y = -1.0f;
                                }
                                if (cm->facing == DIRECTION_DOWN) {
                                    moveDir.y = 1.0f;
                                }
                                if (cm->facing == DIRECTION_LEFT) {
                                    moveDir.x = -1.0f;
                                }
                                if (cm->facing == DIRECTION_RIGHT) {
                                    moveDir.x = 1.0f;
                                }

                                soundManPlaySound("dash");
                                cm->state = CARDMAN_STATE_DASHING;
                                cm->dashVel = vec2ScalarMul(-1.0f * getBackstepSpeedForLevel(cm->backstepLevel), moveDir);
                                cm->dodgedAttackDuringThisDodge = false;
                                cm->dashTimer = 0.3f;
                                setAnimState(cm, getCardmanBackstepAnim(cm));
                            }
                        }
                    }
                } break;
                case CARDMAN_STATE_DASHING: {
                    if (cm->dashTimer <= 0.2f) {
                        if (aobInput.attack.justPressed) {
                            cm->state = CARDMAN_STATE_ATTACKING;
                            setCardmanAttackInfo(cm);
                            setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                            startAnimState(&cm->animState);
                        }
                    }
                } break;
            }
        }
        else {
            switch (cm->state) {
                case CARDMAN_STATE_IDLE: {
                    cm->vel = (vec2){0};
                    if (cm->cpu.currentAction == CPU_ACTION_WAIT) {
                        cm->cpu.timer -= dt;
                        if (cm->cpu.timer <= 0.0f) {
                            cm->state = CARDMAN_STATE_WALKING;
                            setAnimState(cm, getCardmanWalkingAnim(cm));
                        }
                    }
                    else {
                        cm->state = CARDMAN_STATE_WALKING;
                        setAnimState(cm, getCardmanWalkingAnim(cm));
                    }
                } break;
                case CARDMAN_STATE_WALKING: {
                    cardman *player = aob->playerCardman;

                    vec2 cmToPlayer = vec2Subtract(player->pos, cm->pos);

                    if (cm->cpu.timer <= 0.0f) {
                        f32 attackRange = 0.0f;
                        switch (cm->suit) {
                        case CARD_SUIT_SPADE: 
                            attackRange = 60.0f;
                            break;
                        case CARD_SUIT_CLUB: 
                            attackRange = 50.0f;
                            break;
                        case CARD_SUIT_HEART: 
                            attackRange = 100.0f;
                            break;
                        case CARD_SUIT_DIAMOND: 
                            attackRange = 150.0f;
                            break;
                        }

                        vec2 moveDir = vec2Normalize(cmToPlayer);
                        cm->facing = getCPUCardmanFacingForMoveDir(cm, moveDir);

                        f32 distToPlayer = vec2Length(cmToPlayer);
                        if (cm->suit == CARD_SUIT_JOKER) {
                            if (distToPlayer < 100.0f) {
                                cm->cpu.currentAction = CPU_ACTION_WALK_AWAY_FROM_PLAYER;
                                cm->cpu.timer = 1.0f + 1.0f * randomF32();
                            }
                            else {
                                f32 randomRoll = randomF32();
                                if (randomRoll < 0.8f) {
                                    cm->cpu.currentAction = CPU_ACTION_WALK_TOWARDS_PLAYER;
                                    cm->cpu.timer = 0.25f + 0.25f * randomF32();
                                    break;
                                }
                                else {
                                    cm->cpu.currentAction = CPU_ACTION_WAIT;
                                    cm->cpu.timer = 0.3f + 0.2f * randomF32();
                                    cm->state = CARDMAN_STATE_IDLE;
                                    setAnimState(cm, getCardmanIdleAnim(cm));
                                    break;
                                }
                            }
                        }
                        else if (cm->suit == CARD_SUIT_DIAMOND) {
                            if (distToPlayer > attackRange) {
                                while (true) {
                                    f32 randomRoll = randomF32();
                                    if (randomRoll < 0.7f) {
                                        cm->cpu.currentAction = CPU_ACTION_WALK_TOWARDS_PLAYER;
                                        cm->cpu.timer = 1.0f + 3.0f * randomF32();
                                        break;
                                    }
                                    else if (randomRoll < 0.8) {
                                        cm->cpu.currentAction = CPU_ACTION_WAIT;
                                        cm->cpu.timer = 0.5f + 1.0f * randomF32();
                                        cm->state = CARDMAN_STATE_IDLE;
                                        setAnimState(cm, getCardmanIdleAnim(cm));
                                        break;
                                    }
                                    else {
                                        if (cm->dashLevel > 0) {
                                            soundManPlaySound("dash");
                                            cm->state = CARDMAN_STATE_DASHING;
                                            cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                            cm->dodgedAttackDuringThisDodge = false;
                                            cm->dashTimer = 0.3f;
                                            setAnimState(cm, getCardmanDashingAnim(cm));
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (distToPlayer < 60.0f) {
                                f32 randomRoll = randomF32();
                                if (randomRoll < 0.7f) {
                                    cm->cpu.currentAction = CPU_ACTION_WALK_AWAY_FROM_PLAYER;
                                    cm->cpu.timer = 1.0f + 1.0f * randomF32();
                                    break;
                                }
                                else if (randomRoll < 0.8) {
                                    cm->cpu.currentAction = CPU_ACTION_WAIT;
                                    cm->cpu.timer = 0.25f + 0.5f * randomF32();
                                    cm->state = CARDMAN_STATE_IDLE;
                                    setAnimState(cm, getCardmanIdleAnim(cm));
                                    break;
                                }
                                else {
                                    if (cm->dashLevel > 0) {
                                        soundManPlaySound("dash");
                                        cm->state = CARDMAN_STATE_DASHING;
                                        cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), vec2Negate(moveDir));
                                        cm->dodgedAttackDuringThisDodge = false;
                                        cm->dashTimer = 0.3f;
                                        setAnimState(cm, getCardmanDashingAnim(cm));
                                        break;
                                    }
                                }
                            }
                            else {
                                if ((cm->pos.y > player->pos.y - 20.0f && cm->pos.y < player->pos.y + 60.0f) ||
                                    (cm->pos.x > player->pos.x - 40.0f && cm->pos.x < player->pos.x + 40.0f)) 
                                {
                                    f32 randomRoll = randomF32();
                                    if (randomRoll < 0.1f) {
                                        cm->cpu.currentAction = CPU_ACTION_WALK_AWAY_FROM_PLAYER;
                                        cm->cpu.timer = 0.125f + 0.25f * randomF32();
                                        break;
                                    }
                                    if (randomRoll < 0.2f) {
                                        cm->cpu.currentAction = CPU_ACTION_WALK_TOWARDS_PLAYER;
                                        cm->cpu.timer = 0.125f + 0.25f * randomF32();
                                        break;
                                    }
                                    else if (randomRoll < 0.8) {
                                        cm->facing = getCPUCardmanFacingForMoveDir(cm, moveDir);
                                        cm->state = CARDMAN_STATE_ATTACKING;
                                        setCardmanAttackInfo(cm);
                                        setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                                        startAnimState(&cm->animState);
                                        break;
                                    }
                                    else {
                                        cm->cpu.currentAction = CPU_ACTION_WAIT;
                                        cm->cpu.timer = 0.3f + 0.2f * randomF32();
                                        cm->state = CARDMAN_STATE_IDLE;
                                        setAnimState(cm, getCardmanIdleAnim(cm));
                                        break;
                                    }
                                }
                            }
                        }
                        else {
                            if (distToPlayer > attackRange * 1.5f) {
                                while (true) {
                                    f32 randomRoll = randomF32();
                                    if (randomRoll < 0.7f) {
                                        cm->cpu.currentAction = CPU_ACTION_WALK_TOWARDS_PLAYER;
                                        cm->cpu.timer = 1.0f + 3.0f * randomF32();
                                        break;
                                    }
                                    else if (randomRoll < 0.8) {
                                        cm->cpu.currentAction = CPU_ACTION_WAIT;
                                        cm->cpu.timer = 0.5f + 1.0f * randomF32();
                                        cm->state = CARDMAN_STATE_IDLE;
                                        setAnimState(cm, getCardmanIdleAnim(cm));
                                        break;
                                    }
                                    else {
                                        if (cm->dashLevel > 0) {
                                            soundManPlaySound("dash");
                                            cm->state = CARDMAN_STATE_DASHING;
                                            cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                            cm->dodgedAttackDuringThisDodge = false;
                                            cm->dashTimer = 0.3f;
                                            setAnimState(cm, getCardmanDashingAnim(cm));
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (distToPlayer > attackRange) {
                                while (true) {
                                    f32 randomRoll = randomF32();
                                    if (randomRoll < 0.2f) {
                                        cm->cpu.currentAction = CPU_ACTION_WAIT;
                                        cm->cpu.timer = 0.25f + 0.5f * randomF32();
                                        cm->state = CARDMAN_STATE_IDLE;
                                        setAnimState(cm, getCardmanIdleAnim(cm));
                                        break;
                                    }
                                    else if (randomRoll < 0.5f) {
                                        cm->cpu.currentAction = CPU_ACTION_WALK_TOWARDS_PLAYER;
                                        cm->cpu.timer = 0.1f + 0.5f * randomF32();
                                        break;
                                    }
                                    else if (randomRoll < 0.6f) {
                                        if (cm->backstepLevel > 0) {
                                            soundManPlaySound("dash");
                                            cm->state = CARDMAN_STATE_DASHING;
                                            cm->dashVel = vec2ScalarMul(-1.0f * getBackstepSpeedForLevel(cm->backstepLevel), moveDir);
                                            cm->dodgedAttackDuringThisDodge = false;
                                            cm->dashTimer = 0.3f;
                                            setAnimState(cm, getCardmanBackstepAnim(cm));
                                            break;
                                        }
                                    }
                                    else if (randomRoll < 0.7f) {
                                        if (cm->dashLevel > 0) {
                                            soundManPlaySound("dash");
                                            cm->state = CARDMAN_STATE_DASHING;
                                            cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                            cm->dodgedAttackDuringThisDodge = false;
                                            cm->dashTimer = 0.3f;
                                            setAnimState(cm, getCardmanDashingAnim(cm));
                                            break;
                                        }
                                    }
                                    else {
                                        cm->state = CARDMAN_STATE_ATTACKING;
                                        setCardmanAttackInfo(cm);
                                        setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                                        startAnimState(&cm->animState);
                                        break;
                                    }
                                }
                            }
                            else {
                                while (true) {
                                    f32 randomRoll = randomF32();
                                    if (randomRoll < 0.4f) {
                                        cm->cpu.currentAction = CPU_ACTION_WAIT;
                                        cm->cpu.timer = 0.15f + 0.25f * randomF32();
                                        cm->state = CARDMAN_STATE_IDLE;
                                        setAnimState(cm, getCardmanIdleAnim(cm));
                                        break;
                                    }
                                    else if (randomRoll < 0.5f) {
                                        if (cm->backstepLevel > 0) {
                                            soundManPlaySound("dash");
                                            cm->state = CARDMAN_STATE_DASHING;
                                            cm->dashVel = vec2ScalarMul(-1.0f * getBackstepSpeedForLevel(cm->backstepLevel), moveDir);
                                            cm->dodgedAttackDuringThisDodge = false;
                                            cm->dashTimer = 0.3f;
                                            setAnimState(cm, getCardmanBackstepAnim(cm));
                                            break;
                                        }
                                    }
                                    else {
                                        cm->state = CARDMAN_STATE_ATTACKING;
                                        setCardmanAttackInfo(cm);
                                        setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                                        startAnimState(&cm->animState);
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    f32 enemySpeed = getEnemySpeedForLevel(cm->speedLevel);
                    if (cm->cpu.currentAction == CPU_ACTION_WALK_TOWARDS_PLAYER) {
                        cm->cpu.timer -= dt;

                        vec2 moveDir = vec2Normalize(cmToPlayer);
                        cm->facing = getCPUCardmanFacingForMoveDir(cm, moveDir);
                        f32 distToPlayer = vec2Length(cmToPlayer);
                        cm->vel = vec2ScalarMul(enemySpeed, moveDir);

                        setAnimStateWithSpeed(cm, getCardmanWalkingAnim(cm), enemySpeed / 150.0f);

                        if (cm->suit != CARD_SUIT_JOKER && vec2Length(cmToPlayer) < 60.0f && randomF32() < 0.1f) {
                            cm->state = CARDMAN_STATE_ATTACKING;
                            setCardmanAttackInfo(cm);
                            setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                            startAnimState(&cm->animState);
                        }
                    }
                    else if (cm->cpu.currentAction == CPU_ACTION_WALK_AWAY_FROM_PLAYER) {
                        cm->cpu.timer -= dt;

                        vec2 moveDir = vec2Normalize(cmToPlayer);
                        cm->facing = getCPUCardmanFacingForMoveDir(cm, moveDir);
                        f32 distToPlayer = vec2Length(cmToPlayer);
                        cm->vel = vec2ScalarMul(enemySpeed, vec2Negate(moveDir));

                        setAnimStateWithSpeed(cm, getCardmanWalkingAnim(cm), enemySpeed / 150.0f);
                    }
                } break;
                case CARDMAN_STATE_ATTACKING: {
                    cm->cpu.timer = 0.0f; // force re-evaluate
                    cm->vel = (vec2){
                        .x = 0.0f,
                        .y = 0.0f
                    };
                    if (cm->suit == CARD_SUIT_DIAMOND && cm->animState.totalFrames >= 40.0f && !cm->attack.spawnedBullets) {
                        cm->attack.spawnedBullets = true;
                        spawnCardmanBullets(cm);
                    }

                    if (cm->animState.totalFrames >= cm->attack.linkStartFrame) {
                        f32 randomRoll = randomF32();
                        if (randomRoll < 0.8f) {

                        }
                        else if (randomRoll < 0.9f) {
                            if (cm->attackComboLevel > cm->attack.comboIndex) {
                                cm->state = CARDMAN_STATE_ATTACKING;
                                setCardmanAttackInfo(cm);
                                setAnimStateWithSpeed(cm, cm->attack.animKey, cm->attack.speedMultiplier);
                                startAnimState(&cm->animState);
                            }
                        }
                        else if (randomRoll < 0.95f) {
                            if (cm->dashLevel > 0) {
                                vec2 moveDir = {0};
                                if (cm->facing == DIRECTION_UP) {
                                    moveDir.y = -1.0f;
                                }
                                if (cm->facing == DIRECTION_DOWN) {
                                    moveDir.y = 1.0f;
                                }
                                if (cm->facing == DIRECTION_LEFT) {
                                    moveDir.x = -1.0f;
                                }
                                if (cm->facing == DIRECTION_RIGHT) {
                                    moveDir.x = 1.0f;
                                }

                                soundManPlaySound("dash");
                                cm->state = CARDMAN_STATE_DASHING;
                                cm->dashVel = vec2ScalarMul(getDashSpeedForLevel(cm->dashLevel), moveDir);
                                cm->dodgedAttackDuringThisDodge = false;
                                cm->dashTimer = 0.3f;
                                setAnimState(cm, getCardmanBackstepAnim(cm));
                            }
                        }
                        else {
                            if (cm->backstepLevel > 0) {
                                vec2 moveDir = {0};
                                if (cm->facing == DIRECTION_UP) {
                                    moveDir.y = -1.0f;
                                }
                                if (cm->facing == DIRECTION_DOWN) {
                                    moveDir.y = 1.0f;
                                }
                                if (cm->facing == DIRECTION_LEFT) {
                                    moveDir.x = -1.0f;
                                }
                                if (cm->facing == DIRECTION_RIGHT) {
                                    moveDir.x = 1.0f;
                                }

                                soundManPlaySound("dash");
                                cm->state = CARDMAN_STATE_DASHING;
                                cm->dashVel = vec2ScalarMul(-1.0f * getBackstepSpeedForLevel(cm->backstepLevel), moveDir);
                                cm->dodgedAttackDuringThisDodge = false;
                                cm->dashTimer = 0.3f;
                                setAnimState(cm, getCardmanBackstepAnim(cm));
                            }
                        }
                    }
                } break;
            }
        }
    }

    // update actions/movement
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        cm->iframesTimer -= dt;
        if (cm->iframesTimer < 0.0f) {
            cm->iframesTimer = 0.0f;
        }

        cm->knockbackTimer -= dt;
        if (cm->knockbackTimer < 0.0f) {
            cm->knockbackTimer = 0.0f;
            cm->knockbackVel = (vec2){0};
        }
        else {
            cm->pos = vec2Add(cm->pos, vec2ScalarMul(dt, cm->knockbackVel));
        }

        cm->dashTimer -= dt;
        if (cm->dashTimer < 0.0f) {
            cm->dashTimer = 0.0f;
            cm->dashVel = (vec2){0};
            cm->dodgedAttackDuringThisDodge = false;
        }
        else {
            cm->pos = vec2Add(cm->pos, vec2ScalarMul(dt, cm->dashVel));
        }

        switch (cm->state) {
            case CARDMAN_STATE_WALKING: {
                cm->pos = vec2Add(cm->pos, vec2ScalarMul(dt, cm->vel));
            } break;
            case CARDMAN_STATE_DASHING: {
                if (cm->dashTimer <= 0.0f) {
                    cm->state = CARDMAN_STATE_IDLE;
                    setAnimState(cm, getCardmanIdleAnim(cm));
                }
            } break;
            case CARDMAN_STATE_HITSTUN: {
                cm->pos = vec2Add(cm->pos, vec2ScalarMul(dt, cm->vel));
                cm->hitstunTimer -= dt;
                if (cm->hitstunTimer <= 0.0f) {
                    cm->state = CARDMAN_STATE_IDLE;
                    setAnimState(cm, getCardmanIdleAnim(cm));
                }
            } break;
            case CARDMAN_STATE_DEFEATED: {
                cm->defeatedTimer -= dt;
                if (cm->defeatedTimer <= 0.0f) {
                    cm->state = CARDMAN_STATE_FADING_OUT;
                    cm->fadingTimer = 0.5f;
                }
            } break;
            case CARDMAN_STATE_FADING_OUT: {
                cm->fadingTimer -= dt;
                if (cm->fadingTimer <= 0.0f) {
                    cm->active = false;
                    if (cm == aob->playerCardman) {
                        aob->phase = AOB_PHASE_GAMEOVER;
                    }
                }
            } break;
        }
    }

    cardman *player = aob->playerCardman;
    if (player->state == CARDMAN_STATE_ATTACKING) {
        animation_state *playerAnimState = &player->animState;
        char_anim_data *playerAnimData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, playerAnimState->key);
        char_frame_data *playerCurrentFrame = &playerAnimData->frames[playerAnimState->currentFrame];
        mat3x3 playerTransform = mat3x3Translate(player->pos.x, player->pos.y);

        for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
            cardman *enemy = &aob->cardmen[cardmanIndex];

            if (!enemy->active || enemy == player || enemy->iframesTimer > 0.0f || enemy->defeatedTimer > 0.0f || enemy->fadingTimer > 0.0f) {
                continue;
            }

            if (enemy->lastAttackHitByID == player->attack.id) {
                continue;
            }

            b32 hit = testCardmanAttackHitCardman(enemy, playerCurrentFrame, &playerTransform);

            if (hit) {
                if (enemy->dashTimer > 0.0f && !enemy->dodgedAttackDuringThisDodge) {
                    enemy->dodgedAttackDuringThisDodge = true;
                    enemy->lastAttackHitByID = player->attack.id;
                }
                else {
                    enemy->hitByInfo = (cardman_hitby_info){
                        .wasHit = true,
                        .attackID = player->attack.id,
                        .attackOrigin = player->pos,
                        .damage = player->attack.damage * player->attack.damageMultiplier,
                        .knockbackSpeed = player->attack.knockbackMultiplier * getKnockbackForLevel(player->knockbackLevel)
                    };
                    aob->lastHitEnemy = enemy;
                }
            }
        }
    }

    if (player->iframesTimer <= 0.0f && player->suit != CARD_SUIT_JOKER && player->defeatedTimer <= 0.0f && player->fadingTimer <= 0.0f) {
        for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
            cardman *enemy = &aob->cardmen[cardmanIndex];

            if (!enemy->active || enemy == player) {
                continue;
            }

            if (player->lastAttackHitByID == enemy->attack.id) {
                continue;
            }

            if (enemy->state == CARDMAN_STATE_ATTACKING) {
                animation_state *enemyAnimState = &enemy->animState;
                char_anim_data *enemyAnimData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, enemyAnimState->key);
                char_frame_data *enemyCurrentFrame = &enemyAnimData->frames[enemyAnimState->currentFrame];
                mat3x3 enemyTransform = mat3x3Translate(enemy->pos.x, enemy->pos.y);

                b32 hit = testCardmanAttackHitCardman(player, enemyCurrentFrame, &enemyTransform);

                if (hit) {
                    if (player->dashTimer > 0.0f && !player->dodgedAttackDuringThisDodge) {
                        player->dodgedAttackDuringThisDodge = true;
                        player->lastAttackHitByID = enemy->attack.id;
                    }
                    else {
                        player->hitByInfo = (cardman_hitby_info){
                            .wasHit = true,
                            .attackID = enemy->attack.id,
                            .attackOrigin = enemy->pos,
                            .damage = enemy->attack.damage * enemy->attack.damageMultiplier,
                            .knockbackSpeed = enemy->attack.knockbackMultiplier * getKnockbackForLevel(enemy->knockbackLevel)
                        };
                    }
                }
            }
        }
    }

    for (u32 i = 0; i < MAX_NUM_BULLETS; i++) {
        bullet_star *bullet = &aob->bullets[i];
        if (bullet->active) {
            b32 dead = false;

            bullet->lifetime += dt;
            if (bullet->lifetime >= BULLET_LIFETIME) {
                dead = true;
            }
            bullet->pos = vec2Add(bullet->pos, vec2ScalarMul(dt, bullet->vel)); 

            animation_state *bulletAnimState = &bullet->animState;
            char_anim_data *bulletAnimData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, bulletAnimState->key);
            char_frame_data *bulletCurrentFrame = &bulletAnimData->frames[bulletAnimState->currentFrame];
            mat3x3 bulletTransform = mat3x3Translate(bullet->pos.x, bullet->pos.y);

            if (bullet->owner == CARDMAN_OWNER_PLAYER) {
                for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
                    cardman *enemy = &aob->cardmen[cardmanIndex];

                    if (!enemy->active || enemy == player || enemy->iframesTimer > 0.0f) {
                        continue;
                    }
                    if (enemy->lastAttackHitByID == bullet->attackID) {
                        continue;
                    }

                    b32 hit = testCardmanAttackHitCardman(enemy, bulletCurrentFrame, &bulletTransform);

                    if (hit) {
                        if (enemy->dashTimer > 0.0f && !enemy->dodgedAttackDuringThisDodge) {
                            enemy->dodgedAttackDuringThisDodge = true;
                            enemy->lastAttackHitByID = bullet->attackID;
                        }
                        else {
                            enemy->hitByInfo = (cardman_hitby_info){
                                .wasHit = true,
                                .attackID = bullet->attackID,
                                .attackOrigin = bullet->pos,
                                .damage = bullet->damage,
                                .knockbackSpeed = getKnockbackForLevel(player->knockbackLevel)
                            };
                            aob->lastHitEnemy = enemy;

                            bullet->numHitCardmen++;
                            if (bullet->numHitCardmen >= 3) {
                                dead = true;
                            }
                        }
                    }
                }
            }
            else {
                if (player->iframesTimer <= 0.0f && player->suit != CARD_SUIT_JOKER && player->lastAttackHitByID != bullet->attackID) {
                    b32 hit = testCardmanAttackHitCardman(player, bulletCurrentFrame, &bulletTransform);

                    if (hit) {
                        if (player->dashTimer > 0.0f && !player->dodgedAttackDuringThisDodge) {
                            player->dodgedAttackDuringThisDodge = true;
                            player->lastAttackHitByID = bullet->attackID;
                        }
                        else {
                            player->hitByInfo = (cardman_hitby_info){
                                .wasHit = true,
                                .attackID = bullet->attackID,
                                .attackOrigin = bullet->pos,
                                .damage = bullet->damage,
                                .knockbackSpeed = getKnockbackForLevel(1)
                            };
                            dead = true;
                        }
                    }
                }
            }

            bullet->active = !dead;
        }
    }


    u32 maxCollisions = 200;
    saveScratchMemPointer(scratchMemory, &aob->scratchMemSave);
    cardman_collision_list collisions = cardman_collision_listInit(scratchMemory, maxCollisions);

    // TODO: space partition
    // nudge cardmen away from each other
    for (u32 firstIndex = 0; firstIndex < MAX_NUM_CARDMEN; firstIndex++) {
        cardman *first = &aob->cardmen[firstIndex];
        if (!first->active) {
            continue;
        }

        for (u32 secondIndex = 0; secondIndex < MAX_NUM_CARDMEN; secondIndex++) {
            cardman *second = &aob->cardmen[secondIndex];
            if (!second->active || first == second) {
                continue;
            }

            vec2 distBetweenCM = vec2Subtract(second->pos, first->pos);
            if (vec2Length(distBetweenCM) < 35.0f) {
                cardman_collision_listPush(&collisions, (cardman_collision){
                    .first = first,
                    .second = second
                });
                if (collisions.numValues >= maxCollisions) {
                    goto cmCollisionCheckDone;
                }
            }
        }
    }
cmCollisionCheckDone:

    for (u32 collisionIndex = 0; collisionIndex < collisions.numValues; collisionIndex++) {
        cardman_collision *collision = &collisions.values[collisionIndex];
        cardman *first = collision->first;
        cardman *second = collision->second;

        vec2 distBetweenCM = vec2Subtract(collision->second->pos, collision->first->pos);
        vec2 separationDir = vec2Normalize(distBetweenCM);
        first->pos = vec2Add(vec2ScalarMul(-10.0f * dt, separationDir), first->pos);
        second->pos = vec2Add(vec2ScalarMul(10.0f * dt, separationDir), second->pos);

        if (first->owner == CARDMAN_OWNER_PLAYER && aob->jokerTimer > 0.0f) {
            second->hitByInfo = (cardman_hitby_info){
                .wasHit = true,
                .attackID = 0,
                .attackOrigin = first->pos,
                .damage = 3.0f,
                .knockbackSpeed = getKnockbackForLevel(first->knockbackLevel)
            };
            aob->lastHitEnemy = second;
        }
        else if (second->owner == CARDMAN_OWNER_PLAYER && aob->jokerTimer > 0.0f) {
            first->hitByInfo = (cardman_hitby_info){
                .wasHit = true,
                .attackID = 0,
                .attackOrigin = second->pos,
                .damage = 3.0f,
                .knockbackSpeed = getKnockbackForLevel(second->knockbackLevel)
            };
            aob->lastHitEnemy = first;
        }
    }
    restoreScratchMemPointer(scratchMemory, &aob->scratchMemSave);

    // Process cardmen that were hit by attacks
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            continue;
        }

        if (cm->hitByInfo.wasHit) {
            soundManPlaySound("impact");
            cm->lastAttackHitByID = cm->hitByInfo.attackID;
            cm->state = CARDMAN_STATE_HITSTUN;
            setAnimState(cm, getCardmanHitstunAnim(cm));

            cm->dashTimer = 0.0f;
            cm->dashVel = (vec2){0};
            cm->dodgedAttackDuringThisDodge = false;

            vec2 knockBackDir = vec2Subtract(cm->pos, cm->hitByInfo.attackOrigin);
            knockBackDir = vec2Normalize(knockBackDir);
            cm->vel = (vec2){0};
            cm->knockbackVel = vec2ScalarMul(cm->hitByInfo.knockbackSpeed, knockBackDir);

            cm->knockbackTimer = 0.05f;

            cm->hitPoints -= cm->hitByInfo.damage;
            if (cm->hitPoints <= 0.0f) {
                cm->state = CARDMAN_STATE_DEFEATED;
                setAnimState(cm, getCardmanHitstunAnim(cm));

                if (cardsCanMatch(aob->playerCardman->suit, aob->playerCardman->value, cm->suit, cm->value)) {
                    cm->defeatedTimer = 6.0f;
                }
                else {
                    cm->defeatedTimer = 1.0f;
                }

                if (cm == aob->lastHitEnemy) {
                    aob->lastHitEnemy = 0;
                }
            }
            else {
                if (cm->owner == CARDMAN_OWNER_PLAYER) {
                    cm->hitstunTimer = getPlayerHitstunForLevel(cm->playerHitstunLevel);
                    cm->iframesTimer = getInvincibilityFramesForLevel(cm->invincibilityFramesLevel);
                }
                else {
                    cm->hitstunTimer = getEnemyHitstunForLevel(aob->playerCardman->enemyHitstunLevel);
                    cm->iframesTimer = 0.1f;
                }
            }
        }
    }

    if (aob->playedCardInfo.active) {
        played_card_info *pcInfo = &aob->playedCardInfo;
        cardman *player = aob->playerCardman;
        pcInfo->t += dt;

        f32 moveToAbovePlayerDuration = 0.2f;
        f32 pauseDuration = 0.25f;
        f32 moveToPlayerDuration = 0.1f;
        f32 playerPauseDuration = 0.5f;

        switch(pcInfo->phase) {
            case PLAYED_CARD_PHASE_MOVE_CARD_ABOVE: {
                f32 currentT = pcInfo->t;
                b32 nextPhase = false;

                if (currentT > moveToAbovePlayerDuration) {
                    currentT = moveToAbovePlayerDuration;
                    nextPhase = true;
                }

                f32 t = currentT / moveToAbovePlayerDuration;
                vec2 abovePlayer = (vec2){ .x = player->pos.x, .y = player->pos.y - 125.0f };
                vec2 startToAbovePlayer = vec2Subtract(abovePlayer, pcInfo->cardStartPos);
                pcInfo->cardPos = vec2Add(pcInfo->cardStartPos, vec2ScalarMul(t, startToAbovePlayer));

                if (nextPhase) {
                    pcInfo->cardStartPos = pcInfo->cardPos;
                    pcInfo->phase = PLAYED_CARD_PHASE_CARD_PAUSE;
                }

            } break;
            case PLAYED_CARD_PHASE_CARD_PAUSE: {
                f32 currentT = pcInfo->t - moveToAbovePlayerDuration;
                b32 nextPhase = false;

                if (currentT > pauseDuration) {
                    currentT = pauseDuration;
                    nextPhase = true;
                }

                if (nextPhase) {
                    pcInfo->phase = PLAYED_CARD_PHASE_MOVE_CARD_TO_PLAYER;
                }
            } break;
            case PLAYED_CARD_PHASE_MOVE_CARD_TO_PLAYER: {
                f32 currentT = pcInfo->t - (moveToAbovePlayerDuration + pauseDuration);
                b32 nextPhase = false;

                if (currentT > moveToPlayerDuration) {
                    currentT = moveToPlayerDuration;
                    nextPhase = true;
                }

                f32 t = currentT / moveToPlayerDuration;
                vec2 startToPlayer = vec2Subtract(player->pos, pcInfo->cardStartPos);
                pcInfo->cardPos = vec2Add(pcInfo->cardStartPos, vec2ScalarMul(t, startToPlayer));

                if (nextPhase) {

                    player_upgrade_listPush(&aob->upgrades, (player_upgrade){
                        .suit = player->suit,
                        .value = player->value,
                    });
                    aob->jokerTimer = 0.0f;
                    soundManPlaySound("level_up");

                    if (pcInfo->suit != CARD_SUIT_JOKER) {
                        powerup_type pu = getUpgradeTypeForSuitValue(pcInfo->suit, pcInfo->value);
                        switch (pu) {
                            case POWERUP_TYPE_SPEED: {
                                player->speedLevel++;
                            } break;
                            case POWERUP_TYPE_HITPOINTS: {
                                player->hitpointLevel++;
                            } break;
                            case POWERUP_TYPE_DASH: {
                                player->dashLevel++;
                            } break;
                            case POWERUP_TYPE_BACKSTEP: {
                                player->backstepLevel++;
                            } break;
                            case POWERUP_TYPE_ATTACK_DAMAGE: {
                                player->attackDamageLevel++;
                            } break;
                            case POWERUP_TYPE_ATTACK_SPEED: {
                                player->attackSpeedLevel++;
                            } break;
                            case POWERUP_TYPE_ATTACK_COMBO: {
                                player->attackComboLevel++;
                            } break;
                            case POWERUP_TYPE_KNOCKBACK: {
                                player->knockbackLevel++;
                            } break;
                            case POWERUP_TYPE_INCREASE_ENEMY_HITSTUN: {
                                player->enemyHitstunLevel++;
                            } break;
                            case POWERUP_TYPE_DECREASE_PLAYER_HITSTUN: {
                                player->playerHitstunLevel++;
                            } break;
                            case POWERUP_TYPE_INVINCIBILITY_FRAMES: {
                                player->invincibilityFramesLevel++;
                            } break;
                        }

                        aob->newUpgradeTimer = 1.0f;
                        aob->newUpgradeType = pu;
                    }
                    else {
                        aob->jokerTimer = 10.0f;
                    }
                    player->maxHitpoints = getHitpointsForLevel(player->hitpointLevel);

                    player->suit = pcInfo->suit;
                    player->value = pcInfo->value;
                    player->hitPoints = player->maxHitpoints;
                    pcInfo->phase = PLAYED_CARD_PHASE_MOVE_PLAYER_PAUSE;


                    setAnimState(player, getCardmanPlayCardEndAnim(player));
                }
            } break;
            case PLAYED_CARD_PHASE_MOVE_PLAYER_PAUSE: {
                f32 currentT = pcInfo->t - (moveToAbovePlayerDuration + pauseDuration + moveToPlayerDuration);
                b32 nextPhase = false;

                if (currentT > playerPauseDuration) {
                    currentT = playerPauseDuration;
                    nextPhase = true;
                }

                if (nextPhase) {
                    pcInfo->active = false;
                    player->state = CARDMAN_STATE_IDLE;
                    setAnimState(player, getCardmanIdleAnim(player));
                }
            } break;
        }
    }

    for (u32 deckIndex = 0; deckIndex < NUM_DECKS; ++deckIndex) {
        deck *d = &aob->decks[deckIndex];
        played_card_to_deck_info *pcInfo = &d->playedCardToDeckInfo;

        if (pcInfo->active) {
            cardman *player = aob->playerCardman;
            pcInfo->t += dt;

            f32 moveToDeckDuration = 0.25f;
            f32 pauseDuration = 1.0f;

            switch(pcInfo->phase) {
                case PLAYED_CARD_TO_DECK_PHASE_MOVE_CARD_TO_DECK: {
                    f32 currentT = pcInfo->t;
                    b32 nextPhase = false;

                    if (currentT > moveToDeckDuration) {
                        currentT = moveToDeckDuration;
                        nextPhase = true;
                    }

                    f32 t = currentT / moveToDeckDuration;
                    vec2 playerToDeck = vec2Subtract(vec2Add(d->pos,(vec2){ .x = 16.0f, .y = 39.0f }), player->pos);
                    pcInfo->cardPos = vec2Add(pcInfo->cardStartPos, vec2ScalarMul(t, playerToDeck));

                    if (nextPhase) {
                        pcInfo->cardStartPos = pcInfo->cardPos;
                        pcInfo->phase = PLAYED_CARD_TO_DECK_PHASE_CARD_PAUSE;

                        player->state = CARDMAN_STATE_IDLE;
                        setAnimState(player, getCardmanIdleAnim(player));
                    }

                } break;
                case PLAYED_CARD_TO_DECK_PHASE_CARD_PAUSE: {
                    f32 currentT = pcInfo->t - moveToDeckDuration;
                    b32 nextPhase = false;

                    if (currentT > pauseDuration) {
                        currentT = pauseDuration;
                        nextPhase = true;
                    }

                    if (nextPhase) {
                        deck_card_listSplice(&d->cards, d->cards.numValues - 1);
                        pcInfo->active = false;
                    }
                } break;
            }
        }
    }

    // update animations
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            continue;
        }

        ASSERT(cm->animState.key != 0);
        if (!stringEquals(cm->animState.key, cm->animState.prevKey)) {
            startAnimState(&cm->animState);
        }
        b32 animDone = updateAnimState(&cm->animState, dt);

        // state transitions on finished animations
        if (animDone) {
            if (cm->state == CARDMAN_STATE_ATTACKING) {
                cm->state = CARDMAN_STATE_IDLE;
                setAnimState(cm, getCardmanIdleAnim(cm));
                startAnimState(&cm->animState);
            }
        }
    }

    if (aob->newUpgradeTimer > 0.0f) {
        aob->newUpgradeTimer -= dt;
    }
}

void drawBox (vec2 origin, rect box, char *boxKey) {
    spriteManPushTransform((sprite_transform){
        .pos = (vec2) {.x = origin.x, .y = origin.y},
        .scale = 1.0f
    });
    spriteManPushTransform((sprite_transform){
        .pos = box.min,
        .scale = 1.0f
    });
    vec2 rectDims = vec2Subtract(box.max, box.min);
    vec2 rectScale = vec2ScalarMul(1.0f / 10.0f, rectDims);
    mat3x3 boxTransform = mat3x3ScaleXY(rectScale.x, rectScale.y);

    spriteManPushMatrix(boxTransform);

    sprite boxSprite = defaultSprite();
    boxSprite.atlasKey = "atlas";
    boxSprite.frameKey = boxKey;
    boxSprite.alpha = 0.3f;
    spriteManAddSprite(boxSprite);

    spriteManPopMatrix();
    spriteManPopMatrix();
    spriteManPopMatrix();
}

void drawHitBoxes (char_frame_data *currentFrame, vec2 origin) {
    for (int i = 0; i < currentFrame->numHitboxes; ++i) {
        rect box = currentFrame->hitboxes[i];
        char *boxKey = "hitbox_frame_data";
        drawBox(origin, box, boxKey);
    }
    for (int i = 0; i < currentFrame->numHurtboxes; ++i) {
        rect box = currentFrame->hurtboxes[i];
        char *boxKey = "hurtbox_frame_data";
        drawBox(origin, box, boxKey);
    }
}

void cardmenBubbleSort (cardman_ptr_list *cardmen) {
    for (int i = 0; i < cardmen->numValues; ++i) {
        for (int j = i; j > 0; --j) {
            cardman *first = cardmen->values[j-1];
            cardman *second = cardmen->values[j];
            if (second->pos.y < first->pos.y) {
                cardmen->values[j-1] = second;
                cardmen->values[j] = first;
            }
        }
    }
}

void drawAceOfBlades (plat_api platAPI, f32 gameScale, mem_arena *scratchMemory) {
    if (aob->phase == AOB_PHASE_TITLE) {
        spriteManAddText((sprite_text){
            .x = 270.0f,
            .y = 160.0f,
            .text = "ace of blades",
            .fontKey = "font"
        });

        spriteManAddText((sprite_text){
            .x = 240.0f,
            .y = 190.0f,
            .text = "press any button to continue",
            .fontKey = "font"
        });
        return;
    }

    if (aob->phase == AOB_PHASE_GAMEOVER) {
        spriteManAddText((sprite_text){
            .x = 240.0f,
            .y = 160.0f,
            .text = "game over",
            .fontKey = "font"
        });
        return;
    }

    if (aob->phase == AOB_PHASE_WIN) {
        spriteManAddText((sprite_text){
            .x = 240.0f,
            .y = 160.0f,
            .text = "congratulations, you win!",
            .fontKey = "font"
        });
        return;
    }

    vec2 cameraOffset = (vec2){ .x = 320.0f, .y = 220.0f };
    spriteManPushTransform((sprite_transform){
        .pos = vec2Subtract(cameraOffset, aob->playerCardman->pos),
        .scale = 1.0f,
    });

    f32 grassWidth = 108.0f;
    f32 grassHeight = 108.0f;

    vec2 windowOffset = (vec2){ 
        .x = ((f32)platAPI.windowWidth / 2.0f) / gameScale,
        .y = ((f32)platAPI.windowHeight / 2.0f) / gameScale
    };

    vec2 upperCorner = vec2Subtract(aob->playerCardman->pos, windowOffset);
    f32 grassStartX = (-2 + (i32)(upperCorner.x / grassWidth)) * grassWidth;
    f32 grassStartY = (-2 + (i32)(upperCorner.y / grassHeight)) * grassHeight;

    i32 numCols = (2.0f * windowOffset.x) / grassWidth + 4;
    i32 numRows = (2.0f * windowOffset.y) / grassHeight + 4;
    if (numCols < 0) { numCols = -numCols; }
    if (numRows < 0) { numRows = -numRows; }
    for (u32 grassRow = 0; grassRow < numRows; grassRow++) {
        for (u32 grassCol = 0; grassCol < numCols; grassCol++) {
            sprite grass = defaultSprite();
            grass.pos.x = grassStartX + grassWidth * (f32)grassCol;
            grass.pos.y = grassStartY + grassHeight * (f32)grassRow;
            grass.atlasKey = "atlas";
            grass.frameKey = "grassy_checkers";
            spriteManAddSprite(grass);
        }
    }

    sprite deckRectSprite = defaultSprite();
    deckRectSprite.pos.x = -50.0f;
    deckRectSprite.pos.y = -50.0f;
    deckRectSprite.atlasKey = "atlas";
    deckRectSprite.frameKey = "deck_rect";
    spriteManAddSprite(deckRectSprite);

    for (u32 i = 0; i < NUM_DECKS; i++) {
        deck *d = &aob->decks[i];
        vec2 pos = d->pos;


        if (d->cards.numValues == 0) {
            sprite cardSprite = defaultSprite();
            cardSprite.pos.x = pos.x;
            cardSprite.pos.y = pos.y;
            cardSprite.atlasKey = "atlas";
            cardSprite.frameKey = "foundation";
            spriteManAddSprite(cardSprite);
        }
        else {
            u32 numBlankCards = d->cards.numValues - 1;
            if (numBlankCards >= 3) {
                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = "card_back";
                spriteManAddSprite(cardSprite);
                pos.y -= 2.0f;
            }
            if (numBlankCards >= 2) {
                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = "card_back";
                spriteManAddSprite(cardSprite);
                pos.y -= 2.0f;
            }
            if (numBlankCards >= 1) {
                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = "card_back";
                spriteManAddSprite(cardSprite);
                pos.y -= 2.0f;
            }

            deck_card topCard = d->cards.values[d->cards.numValues - 1];

            if (cardsCanMatch(aob->playerCardman->suit, aob->playerCardman->value, topCard.suit, topCard.value)) {
                sprite buttonPromptSprite = defaultSprite();
                buttonPromptSprite.pos.x = d->pos.x + 16.0f;
                buttonPromptSprite.pos.y = d->pos.y - 25.0f;
                buttonPromptSprite.anchor = (vec2){ .x = 0.5f, .y = 0.5f };
                buttonPromptSprite.atlasKey = "atlas";
                buttonPromptSprite.frameKey = aob->inputSource == INPUT_SOURCE_KEYBOARD ? "a_key_prompt" : "right_button_prompt";

                buttonPromptSprite.alpha = 0.5f;
                vec2 playerToCM = vec2Subtract(vec2Add(d->pos,(vec2){ .x = 12.0f, .y = 40.0f }), aob->playerCardman->pos);
                f32 distToCM = vec2Length(playerToCM);
                if (distToCM < DIST_TO_PLAY_CARD_TO_DECK) {
                    buttonPromptSprite.alpha = 1.0f;
                }

                spriteManAddSprite(buttonPromptSprite);
            }

            if (d->playedCardToDeckInfo.active) {
                played_card_to_deck_info *pcInfo = &d->playedCardToDeckInfo;
                switch (pcInfo->phase) {
                    case PLAYED_CARD_TO_DECK_PHASE_MOVE_CARD_TO_DECK: {
                        sprite cardSprite = defaultSprite();
                        cardSprite.pos.x = pcInfo->cardPos.x;
                        cardSprite.pos.y = pcInfo->cardPos.y;
                        cardSprite.atlasKey = "atlas";
                        cardSprite.frameKey = getCardSuitFrame(pcInfo->suit);
                        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };
                        spriteManAddSprite(cardSprite);

                        if (pcInfo->suit != CARD_SUIT_JOKER) {
                            sprite valSprite = defaultSprite();
                            valSprite.pos.x = pcInfo->cardPos.x - 12.0f;
                            valSprite.pos.y = pcInfo->cardPos.y - 41.0f;
                            valSprite.atlasKey = "atlas";
                            valSprite.frameKey = suitAndValueToFrame(pcInfo->suit, pcInfo->value);
                            spriteManAddSprite(valSprite);
                        }
                    } break;
                    case PLAYED_CARD_PHASE_CARD_PAUSE: {
                        sprite cardSprite = defaultSprite();
                        cardSprite.pos.x = pcInfo->cardPos.x;
                        cardSprite.pos.y = pcInfo->cardPos.y;
                        cardSprite.atlasKey = "atlas";
                        cardSprite.frameKey = getCardSuitFrame(pcInfo->suit);
                        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };

                        f32 t = pcInfo->t - 0.25f;
                        f32 blinkVal = sin2PI(t * 6.0f);
                        if (blinkVal >= 0.0f) {
                            cardSprite.tint = 0xdd99bb;
                        }
                        spriteManAddSprite(cardSprite);

                        if (pcInfo->suit != CARD_SUIT_JOKER) {
                            sprite valSprite = defaultSprite();
                            valSprite.pos.x = pcInfo->cardPos.x - 12.0f;
                            valSprite.pos.y = pcInfo->cardPos.y - 41.0f;
                            valSprite.atlasKey = "atlas";
                            valSprite.frameKey = suitAndValueToFrame(pcInfo->suit, pcInfo->value);
                            spriteManAddSprite(valSprite);
                        }
                    } break;
                }

            }
            else {

                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = getCardSuitFrame(topCard.suit);
                spriteManAddSprite(cardSprite);

                if (topCard.suit != CARD_SUIT_JOKER) {
                    sprite valSprite = defaultSprite();
                    valSprite.pos.x = pos.x + 3.0f;
                    valSprite.pos.y = pos.y + 3.0f;
                    valSprite.atlasKey = "atlas";
                    valSprite.frameKey = suitAndValueToFrame(topCard.suit, topCard.value);
                    spriteManAddSprite(valSprite);
                }
            }
        }
    }

    cardman_ptr_list cardmen = cardman_ptr_listInit(scratchMemory, MAX_NUM_CARDMEN);
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        // TODO: draw player characters on top?
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (cm->active) {
            cardman_ptr_listPush(&cardmen, cm);
        }
    }

    cardmenBubbleSort(&cardmen);

    for (u32 cardmanIndex = 0; cardmanIndex < cardmen.numValues; cardmanIndex++) {
        // TODO: draw player characters on top?
        cardman *cm = cardmen.values[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, cm->animState.key);
        char_frame_data *currentFrame = &animData->frames[cm->animState.currentFrame];

        vec2 cmSpriteOrigin = vec2Add(cm->pos, 
                                      (vec2){ .x = currentFrame->xOffset, .y = currentFrame->yOffset });
        
        sprite cmSprite = defaultSprite();
        cmSprite.pos = cmSpriteOrigin;
        cmSprite.atlasKey = "atlas";
        cmSprite.frameKey = currentFrame->frameKey;
        ASSERT(cmSprite.frameKey != 0);
        cmSprite.anchor = (vec2){ .x = 0.0f, .y = 1.0f };
        if (cm->iframesTimer > 0.0f) {
            cmSprite.tint = 0xff0000;
        }
        if (cm->suit == CARD_SUIT_JOKER && cm->owner == CARDMAN_OWNER_PLAYER) {
            f32 blinkVal = sin2PI(aob->gameTime * 6.0f);
            if (blinkVal >= 0.0f) {
                cmSprite.tint = 0xff88ff;
            }
        }
        if (cm->defeatedTimer > 0.0f) {
            f32 defeatBlinkVal = sin2PI(cm->defeatedTimer * 6.0f);
            if (defeatBlinkVal >= 0.0f) {
                cmSprite.tint = 0xdd99bb;
            }
            if (cardsCanMatch(aob->playerCardman->suit, aob->playerCardman->value, cm->suit, cm->value)) {

                sprite buttonPromptSprite = defaultSprite();
                buttonPromptSprite.pos.x = cm->pos.x;
                buttonPromptSprite.pos.y = cm->pos.y - 85.0f;
                buttonPromptSprite.anchor = (vec2){ .x = 0.5f, .y = 0.5f };
                buttonPromptSprite.atlasKey = "atlas";
                buttonPromptSprite.frameKey = aob->inputSource == INPUT_SOURCE_KEYBOARD ? "a_key_prompt" : "right_button_prompt";

                buttonPromptSprite.alpha = 0.5f;
                vec2 playerToCM = vec2Subtract(aob->playerCardman->pos, cm->pos);
                f32 distToCM = vec2Length(playerToCM);
                if (distToCM < DIST_TO_PLAY_CARD) {
                    buttonPromptSprite.alpha = 1.0f;
                }

                spriteManAddSprite(buttonPromptSprite);
            }
        }

        if (cm->fadingTimer > 0.0f) {
            cmSprite.alpha = 0.5f;
        }

        if (cm->dashTimer > 0.0f) {
            i32 lastPosIndex = cm->lastPosIndex;
            for (u32 i = 0; i < 3; i++) {
                lastPosIndex--;
                if (lastPosIndex < 0) {
                    lastPosIndex = 2;
                }
                sprite shadow = cmSprite;
                vec2 pos = cm->lastPositions[lastPosIndex];
                pos = vec2Add(pos, (vec2){ .x = currentFrame->xOffset, .y = currentFrame->yOffset });
                shadow.pos = pos;
                shadow.tint = 0x000000;
                shadow.alpha = 0.5f;
                spriteManAddSprite(shadow);
            }
        }

        spriteManAddSprite(cmSprite);

        if (cm->suit != CARD_SUIT_JOKER) {
            if (cm->hitstunTimer > 0.0f || 
                cm->defeatedTimer > 0.0f || 
                cm->fadingTimer > 0.0f || 
                cm->facing != DIRECTION_UP) 
            {
                sprite valSprite = defaultSprite();
                valSprite.pos.x = cm->pos.x - 12.0f;
                valSprite.pos.y = cm->pos.y - 55.0f;
                valSprite.atlasKey = "atlas";
                valSprite.frameKey = suitAndValueToFrame(cm->suit, cm->value);
                spriteManAddSprite(valSprite);
            }
        }

        //drawHitBoxes(currentFrame, cm->pos);
    }

    for (u32 i = 0; i < MAX_NUM_BULLETS; i++) {
        bullet_star *bullet = &aob->bullets[i];
        if (bullet->active) {

            char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, bullet->animState.key);
            char_frame_data *currentFrame = &animData->frames[bullet->animState.currentFrame];

            vec2 bulletOrigin = vec2Add(bullet->pos, (vec2){ .x = currentFrame->xOffset, .y = currentFrame->yOffset });
            
            sprite bulletSprite = defaultSprite();
            bulletSprite.pos = bulletOrigin;
            bulletSprite.atlasKey = "atlas";
            bulletSprite.frameKey = currentFrame->frameKey;
            bulletSprite.anchor = (vec2){ .x = 0.0f, .y = 1.0f };
            ASSERT(bulletSprite.frameKey != 0);

            spriteManAddSprite(bulletSprite);
            //drawHitBoxes(currentFrame, bullet->pos);
        }
    }

    if (aob->playedCardInfo.active && aob->playedCardInfo.phase != PLAYED_CARD_PHASE_MOVE_PLAYER_PAUSE) {
        sprite cardSprite = defaultSprite();
        cardSprite.pos.x = aob->playedCardInfo.cardPos.x;
        cardSprite.pos.y = aob->playedCardInfo.cardPos.y - 25.0f;
        cardSprite.atlasKey = "atlas";
        cardSprite.frameKey = getCardSuitFrame(aob->playedCardInfo.suit);
        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };
        spriteManAddSprite(cardSprite);

        if (aob->playedCardInfo.suit != CARD_SUIT_JOKER) {
            sprite valSprite = defaultSprite();
            valSprite.pos.x = aob->playedCardInfo.cardPos.x - 12.0f;
            valSprite.pos.y = aob->playedCardInfo.cardPos.y - 55.0f;
            valSprite.atlasKey = "atlas";
            valSprite.frameKey = suitAndValueToFrame(aob->playedCardInfo.suit, aob->playedCardInfo.value);
            spriteManAddSprite(valSprite);
        }
    }

    spriteManPopMatrix();

    sprite topBannerSprite = defaultSprite();
    topBannerSprite.pos.x = 88.0f;
    topBannerSprite.pos.y = 0.0f;
    topBannerSprite.atlasKey = "atlas";
    topBannerSprite.frameKey = "top_banner";
    spriteManAddSprite(topBannerSprite);

    spriteManAddText((sprite_text){
        .x = 96.0f,
        .y = 11.0f,
        .text = "player",
        .fontKey = "font"
    });

    spriteManAddText((sprite_text){
        .x = 510.0f,
        .y = 11.0f,
        .text = "enemy",
        .fontKey = "font"
    });

    mat3x3 posMatrix = mat3x3Translate(147.0f, 3.0f);
    spriteManPushMatrix(posMatrix);

    // TODO: maxHealth prop
    f32 healthPercent = aob->playerCardman->hitPoints / aob->playerCardman->maxHitpoints;
    if (healthPercent < 0.0f) { healthPercent = 0.0f; }
    f32 healthWidth = (1.0f / 22.0f) * healthPercent * 135.0f;

    mat3x3 scaleTransform = mat3x3ScaleXY(healthWidth, 1.0f);
    spriteManPushMatrix(scaleTransform);
    
    sprite healthBarSprite = defaultSprite();
    healthBarSprite.atlasKey = "atlas";
    healthBarSprite.frameKey = "health_square";

    spriteManAddSprite(healthBarSprite);
    spriteManPopMatrix();
    spriteManPopMatrix();

    if (aob->lastHitEnemy != 0) {
        mat3x3 posMatrix = mat3x3Translate(503.0f, 3.0f);
        spriteManPushMatrix(posMatrix);

        f32 healthPercent = aob->lastHitEnemy->hitPoints / aob->lastHitEnemy->maxHitpoints;
        if (healthPercent < 0.0f) { healthPercent = 0.0f; }
        f32 healthWidth = (1.0f / 22.0f) * healthPercent * 135.0f;

        mat3x3 scaleTransform = mat3x3ScaleXY(-healthWidth, 1.0f);
        spriteManPushMatrix(scaleTransform);
        
        sprite healthBarSprite = defaultSprite();
        healthBarSprite.atlasKey = "atlas";
        healthBarSprite.frameKey = "health_square";

        spriteManAddSprite(healthBarSprite);
        spriteManPopMatrix();
        spriteManPopMatrix();
    }

    u32 time = (u32)aob->gameTime;
    u32 seconds = time % 60;
    u32 minutes = time / 60;
    u32 hours = time / 3600;
    char *timeString = tempStringFromI32((i32)hours);
    timeString = tempStringAppend(timeString, ":");
    if (minutes < 10) {
        timeString = tempStringAppend(timeString, "0");
    }
    timeString = tempStringAppend(timeString, tempStringFromI32((i32)minutes));
    timeString = tempStringAppend(timeString, ":");
    if (seconds < 10) {
        timeString = tempStringAppend(timeString, "0");
    }
    timeString = tempStringAppend(timeString, tempStringFromI32((i32)seconds));

    spriteManAddText((sprite_text){
        .x = 302.0f,
        .y = 11.0f,
        .text = timeString,
        .fontKey = "font"
    });

    if (aob->newUpgradeTimer > 0.0f) {
        sprite upgradeBannerSprite = defaultSprite();
        upgradeBannerSprite.pos.x = 320.0f;
        upgradeBannerSprite.pos.y = 180.0f;
        upgradeBannerSprite.anchor.x = 0.5f;
        upgradeBannerSprite.atlasKey = "atlas";
        upgradeBannerSprite.frameKey = "upgrade_panel";

        if (aob->newUpgradeTimer > 0.7f) {
            f32 t = 1.0f - ((aob->newUpgradeTimer - 0.7f) / 0.3f);
            upgradeBannerSprite.alpha = t;
            if (upgradeBannerSprite.alpha == 0.0f) {
                upgradeBannerSprite.alpha = 0.001f;
            }
            upgradeBannerSprite.pos.y = 170.0f - t * 40.0f;
        }
        else if (aob->newUpgradeTimer > 0.3f) {
            upgradeBannerSprite.pos.y = 130.0f;
        }
        else {
            f32 t = aob->newUpgradeTimer / 0.3f;
            upgradeBannerSprite.alpha = t;
            if (upgradeBannerSprite.alpha == 0.0f) {
                upgradeBannerSprite.alpha = 0.001f;
            }
            upgradeBannerSprite.pos.y = 130.0f - (1.0f - t) * 40.0f;
        }

        spriteManAddSprite(upgradeBannerSprite);

        char *upgradeText = "";
        switch (aob->newUpgradeType) {
        case POWERUP_TYPE_SPEED:
            upgradeText = "+spd";
            break;
        case POWERUP_TYPE_HITPOINTS:
            upgradeText = "+hp";
            break;
        case POWERUP_TYPE_DASH:
            upgradeText = "+dash";
            break;
        case POWERUP_TYPE_BACKSTEP:
            upgradeText = "+bckstp";
            break;
        case POWERUP_TYPE_ATTACK_DAMAGE:
            upgradeText = "+atk";
            break;
        case POWERUP_TYPE_ATTACK_SPEED:
            upgradeText = "+atk spd";
            break;
        case POWERUP_TYPE_ATTACK_COMBO:
            upgradeText = "+combo";
            break;
        case POWERUP_TYPE_KNOCKBACK:
            upgradeText = "+knckbck";
            break;
        case POWERUP_TYPE_INCREASE_ENEMY_HITSTUN:
            upgradeText = "+e.hitstun";
            break;
        case POWERUP_TYPE_DECREASE_PLAYER_HITSTUN:
            upgradeText = "-p.hitstun";
            break;
        case POWERUP_TYPE_INVINCIBILITY_FRAMES:
            upgradeText = "+iframes";
            break;
        }

        spriteManAddText((sprite_text){
            .x = upgradeBannerSprite.pos.x - 30.0f,
            .y = upgradeBannerSprite.pos.y + 8.0f,
            .text = upgradeText,
            .alpha = upgradeBannerSprite.alpha,
            .fontKey = "font"
        });
    }

    if (aob->guideOpen) {
        sprite topBannerSprite = defaultSprite();
        topBannerSprite.pos.x = 64.0f;
        topBannerSprite.pos.y = 68.0f;
        topBannerSprite.textureKey = "guide";
        spriteManAddSprite(topBannerSprite);

        f32 currTextY = 100.0f;
        f32 textInc = 16.0f;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = "updgrade levels:",
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("spd: ", tempStringFromI32(aob->playerCardman->speedLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("hp: ", tempStringFromI32(aob->playerCardman->hitpointLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("dash: ", tempStringFromI32(aob->playerCardman->dashLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("bckstp: ", tempStringFromI32(aob->playerCardman->backstepLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("atk: ", tempStringFromI32(aob->playerCardman->attackDamageLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("atk spd: ", tempStringFromI32(aob->playerCardman->attackSpeedLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("combo: ", tempStringFromI32(aob->playerCardman->attackComboLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("knckbck: ", tempStringFromI32(aob->playerCardman->knockbackLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("e.hitstun: ", tempStringFromI32(aob->playerCardman->enemyHitstunLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("p.hitstun: ", tempStringFromI32(aob->playerCardman->playerHitstunLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
        spriteManAddText((sprite_text){
            .x = 68.0f,
            .y = currTextY,
            .text = tempStringAppend("iframes: ", tempStringFromI32(aob->playerCardman->invincibilityFramesLevel)),
            .fontKey = "font"
        });
        currTextY += textInc;
    }
}

