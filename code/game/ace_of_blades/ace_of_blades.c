#include "ace_of_blades.h"

aob_state *aob;

#include "aob_cardman_anims.c"

void saveScratchMemPointer (mem_arena *scratchMemory, scratch_mem_save *scratchMemSave) {
    scratchMemSave->current = scratchMemory->current;
}

void restoreScratchMemPointer (mem_arena *scratchMemory, scratch_mem_save *scratchMemSave) {
    scratchMemory->current = scratchMemSave->current;
}

card_suit randomSuit () {
    return randomU32() % CARD_SUIT_COUNT;
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
}

void activateCardMan (cardman_owner owner, vec2 pos, deck_card card) {
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            zeroMemory((u8 *)cm, sizeof(cardman));

            *cm = (cardman){
                .active = true,
                .owner = owner,

                .suit = card.suit,
                .value = card.value,

                .state = CARDMAN_STATE_IDLE,
                .facing = DIRECTION_DOWN,
                .pos = pos,
                .hitPoints = 40.0f,
                //.vel
            };

            cm->animState = (animation_state){
                .key = getCardmanIdleAnim(cm),
                .prevKey = getCardmanIdleAnim(cm)
            };

            if (owner == CARDMAN_OWNER_PLAYER) {
                aob->playerCardman = cm;
            }

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

    deck_card_listPush(&aob->decks[0].cards, makeMatchingCard(aob->playerCardman->suit, aob->playerCardman->value));

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
        } break;
        case INPUT_SOURCE_VIRTUAL: {
            result.up = vInput->dPadUp.button;;
            result.down = vInput->dPadDown.button;
            result.left = vInput->dPadLeft.button;
            result.right = vInput->dPadRight.button;
            result.attack = vInput->bottomButton.button;
            result.dash = vInput->leftButton.button;
            result.playCard = vInput->rightButton.button;
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
                    break;
                }
            }
        } break;
    }

    return result;
}

cardman_attack getCardmanAttackInfo (cardman *cm) {
    switch (cm->suit) {
        case CARD_SUIT_SPADE: {
            return (cardman_attack){
                .active = true,
                .animKey = getCardmanKatanaAnim(cm),
                .comboIndex = 0,
                .damage = 15.0f,
                .damageMultiplier = 1.0f,
                .linkStartFrame = 45,
                .baseSpeed = 1.0f,
                .speedMultiplier = 1.0f
            };
        } break;
        case CARD_SUIT_CLUB: {
        } break;
        case CARD_SUIT_DIAMOND: {
        } break;
        case CARD_SUIT_HEART: {
        } break;
    }

    return (cardman_attack){
        .active = true,
        .animKey = getCardmanKatanaAnim(cm),
        .comboIndex = 0,
        .damage = 15.0f,
        .damageMultiplier = 1.0f,
        .linkStartFrame = 45,
        .baseSpeed = 1.0f,
        .speedMultiplier = 1.0f
    };
}

b32 cardsCanMatch (card_suit firstSuit, card_val firstValue, card_suit secondSuit, card_val secondValue) {
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

void tryPlayCard (cardman *cm, mem_arena *scratchMemory) {
    b32 playedCardFromEnemy = false;
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *enemy = &aob->cardmen[cardmanIndex];

        if (!enemy->active || enemy == cm || enemy->defeatedTimer <= 0.0f) {
            continue;
        }
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

                playedCardFromEnemy = true;
                enemy->active = false;
                break;
            }
        }
    }

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

                    d->playedCardToDeckInfo = (played_card_to_deck_info){
                        .active = true,
                        .suit = cm->suit,
                        .value = cm->value,
                        .cardStartPos = cm->pos,
                        .phase = PLAYED_CARD_TO_DECK_PHASE_MOVE_CARD_TO_DECK,
                        .t = 0.0f,
                    };

                    // TODO: remove card from player
                    if (aob->upgrades.numValues == 0) {
                        cm->active = false;
                    }
                    else {
                        player_upgrade pu = aob->upgrades.values[aob->upgrades.numValues - 1];
                        player_upgrade_listSplice(&aob->upgrades, aob->upgrades.numValues - 1);

                        cm->suit = pu.suit;
                        cm->value = pu.value;
                        setAnimState(cm, getCardmanPlayCardEndAnim(cm));
                    }

                    break;
                }
            }
        }
    }
}

b32 testCardmanAttackHitCardman (cardman *attacker, cardman *enemy, 
                                 char_frame_data *attackerFrame, mat3x3 *attackerTransform) 
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


    aob->spawnTimer += dt;
    while (aob->spawnTimer > 2.0f) {
        aob->spawnTimer -= 2.0f;
        f32 matchingChance = 1.0f / 7.0f;
        deck_card card;
        if (randomF32() < matchingChance) {
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

    // update controls/movement
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        zeroMemory((u8 *)&cm->hitByInfo, sizeof(cardman_hitby_info));

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

                    if (aobInput.attack.justPressed) {
                        cm->state = CARDMAN_STATE_ATTACKING;
                        cm->attack = getCardmanAttackInfo(cm);
                        setAnimState(cm, cm->attack.animKey);
                    }
                    if (aobInput.playCard.justPressed) {
                        tryPlayCard(cm, scratchMemory);
                    }
                    if (aobInput.dash.justPressed) {
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

                        cm->state = CARDMAN_STATE_DASHING;
                        cm->dashVel = vec2ScalarMul(-250.0f, moveDir);
                        cm->dashTimer = 0.3f;
                        setAnimState(cm, getCardmanBackstepAnim(cm));
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
                        cm->vel = vec2ScalarMul(CARDMAN_SPEED, moveDir);

                        cm->facing = getCardmanFacingForMoveDir(cm, moveDir);
                        setAnimStateWithSpeed(cm, getCardmanWalkingAnim(cm), CARDMAN_SPEED / 150.0f);

                        if (aobInput.dash.justPressed) {
                            cm->state = CARDMAN_STATE_DASHING;
                            cm->dashVel = vec2ScalarMul(250.0f, moveDir);
                            cm->dashTimer = 0.3f;
                            setAnimState(cm, getCardmanDashingAnim(cm));
                        }
                    }

                    if (aobInput.attack.justPressed) {
                        cm->state = CARDMAN_STATE_ATTACKING;
                        cm->attack = getCardmanAttackInfo(cm);
                        setAnimState(cm, cm->attack.animKey);
                        startAnimState(&cm->animState);
                    }
                    if (aobInput.playCard.justPressed) {
                        tryPlayCard(cm, scratchMemory);
                    }
                } break;
                case CARDMAN_STATE_ATTACKING: {
                    // TODO: linked attack/dash
                    if (cm->animState.totalFrames >= cm->attack.linkStartFrame) {
                        if (aobInput.attack.justPressed) {

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
                            cm->attack = getCardmanAttackInfo(cm);
                            setAnimState(cm, cm->attack.animKey);
                            startAnimState(&cm->animState);
                        }
                    }
                } break;
            }
        }
        else {
            switch (cm->state) {
                case CARDMAN_STATE_IDLE: {
                    cm->state = CARDMAN_STATE_WALKING;
                    setAnimState(cm, getCardmanWalkingAnim(cm));
                } break;
                case CARDMAN_STATE_WALKING: {
                    cardman *player = aob->playerCardman;
                    vec2 cmToPlayer = vec2Subtract(player->pos, cm->pos);
                    vec2 moveDir = vec2Normalize(cmToPlayer);

                    f32 enemySpeed = 30.0f;
                    cm->vel = vec2ScalarMul(enemySpeed, moveDir);

                    cm->facing = getCPUCardmanFacingForMoveDir(cm, moveDir);
                    setAnimStateWithSpeed(cm, getCardmanWalkingAnim(cm), enemySpeed / 150.0f);

                    if (vec2Length(cmToPlayer) < 60.0f) {
                        cm->state = CARDMAN_STATE_ATTACKING;
                        cm->attack = getCardmanAttackInfo(cm);
                        setAnimState(cm, cm->attack.animKey);
                        startAnimState(&cm->animState);
                    }
                } break;
                case CARDMAN_STATE_ATTACKING: {
                    cm->vel = (vec2){
                        .x = 0.0f,
                        .y = 0.0f
                    };
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

            if (!enemy->active || enemy == player || enemy->iframesTimer > 0.0f) {
                continue;
            }

            b32 hit = testCardmanAttackHitCardman(player, enemy, playerCurrentFrame, &playerTransform);

            if (hit) {
                enemy->hitByInfo = (cardman_hitby_info){
                    .wasHit = true,
                    .attackOrigin = player->pos,
                    .damage = player->attack.damage * player->attack.damageMultiplier,
                    .knockbackSpeed = 500.0f
                };
                aob->lastHitEnemy = enemy;
            }
        }
    }

    if (player->iframesTimer <= 0.0f) {
        for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
            cardman *enemy = &aob->cardmen[cardmanIndex];

            if (!enemy->active || enemy == player) {
                continue;
            }

            if (enemy->state == CARDMAN_STATE_ATTACKING) {
                animation_state *enemyAnimState = &enemy->animState;
                char_anim_data *enemyAnimData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, enemyAnimState->key);
                char_frame_data *enemyCurrentFrame = &enemyAnimData->frames[enemyAnimState->currentFrame];
                mat3x3 enemyTransform = mat3x3Translate(enemy->pos.x, enemy->pos.y);

                b32 hit = testCardmanAttackHitCardman(enemy, player, enemyCurrentFrame, &enemyTransform);

                if (hit) {
                    player->hitByInfo = (cardman_hitby_info){
                        .wasHit = true,
                        .attackOrigin = enemy->pos,
                        .damage = enemy->attack.damage * enemy->attack.damageMultiplier,
                        .knockbackSpeed = 500.0f
                    };
                }
            }
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
    }
    restoreScratchMemPointer(scratchMemory, &aob->scratchMemSave);

    // Process cardmen that were hit by attacks
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            continue;
        }

        if (cm->hitByInfo.wasHit) {

            cm->state = CARDMAN_STATE_HITSTUN;
            setAnimState(cm, getCardmanHitstunAnim(cm));

            cm->dashTimer = 0.0f;
            cm->dashVel = (vec2){0};

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
                cm->hitstunTimer = 0.75f;
                cm->iframesTimer = 0.1f;
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

                    player->suit = pcInfo->suit;
                    player->value = pcInfo->value;
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
            if (numBlankCards > 3) {
                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = "card_back";
                spriteManAddSprite(cardSprite);
                pos.y -= 2.0f;
            }
            if (numBlankCards > 2) {
                sprite cardSprite = defaultSprite();
                cardSprite.pos.x = pos.x;
                cardSprite.pos.y = pos.y;
                cardSprite.atlasKey = "atlas";
                cardSprite.frameKey = "card_back";
                spriteManAddSprite(cardSprite);
                pos.y -= 2.0f;
            }
            if (numBlankCards > 1) {
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
                buttonPromptSprite.pos.x = d->pos.x + 12.0f;
                buttonPromptSprite.pos.y = d->pos.y - 35.0f;
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
                        cardSprite.frameKey = getCardSuitFrame(topCard.suit);
                        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };
                        spriteManAddSprite(cardSprite);

                        sprite valSprite = defaultSprite();
                        valSprite.pos.x = pcInfo->cardPos.x - 12.0f;
                        valSprite.pos.y = pcInfo->cardPos.y - 41.0f;
                        valSprite.atlasKey = "atlas";
                        valSprite.frameKey = suitAndValueToFrame(topCard.suit, topCard.value);
                        spriteManAddSprite(valSprite);
                    } break;
                    case PLAYED_CARD_PHASE_CARD_PAUSE: {
                        sprite cardSprite = defaultSprite();
                        cardSprite.pos.x = pcInfo->cardPos.x;
                        cardSprite.pos.y = pcInfo->cardPos.y;
                        cardSprite.atlasKey = "atlas";
                        cardSprite.frameKey = getCardSuitFrame(topCard.suit);
                        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };

                        f32 t = pcInfo->t - 0.25f;
                        f32 blinkVal = sin2PI(t * 6.0f);
                        if (blinkVal >= 0.0f) {
                            cardSprite.tint = 0xdd99bb;
                        }
                        spriteManAddSprite(cardSprite);

                        sprite valSprite = defaultSprite();
                        valSprite.pos.x = pcInfo->cardPos.x - 12.0f;
                        valSprite.pos.y = pcInfo->cardPos.y - 41.0f;
                        valSprite.atlasKey = "atlas";
                        valSprite.frameKey = suitAndValueToFrame(topCard.suit, topCard.value);
                        spriteManAddSprite(valSprite);
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

                sprite valSprite = defaultSprite();
                valSprite.pos.x = pos.x + 3.0f;
                valSprite.pos.y = pos.y + 3.0f;
                valSprite.atlasKey = "atlas";
                valSprite.frameKey = suitAndValueToFrame(topCard.suit, topCard.value);
                spriteManAddSprite(valSprite);
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
                cmSprite.pos = pos;
                cmSprite.tint = 0x000000;
                cmSprite.alpha = 0.5f;
                spriteManAddSprite(shadow);
            }
        }

        spriteManAddSprite(cmSprite);

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
        //drawHitBoxes(currentFrame, cm->pos);
    }

    if (aob->playedCardInfo.active && aob->playedCardInfo.phase != PLAYED_CARD_PHASE_MOVE_PLAYER_PAUSE) {
        sprite cardSprite = defaultSprite();
        cardSprite.pos.x = aob->playedCardInfo.cardPos.x;
        cardSprite.pos.y = aob->playedCardInfo.cardPos.y - 25.0f;
        cardSprite.atlasKey = "atlas";
        cardSprite.frameKey = getCardSuitFrame(aob->playedCardInfo.suit);
        cardSprite.anchor = (vec2){ .x = 0.5f, .y = 1.0f };
        spriteManAddSprite(cardSprite);

        sprite valSprite = defaultSprite();
        valSprite.pos.x = aob->playedCardInfo.cardPos.x - 12.0f;
        valSprite.pos.y = aob->playedCardInfo.cardPos.y - 55.0f;
        valSprite.atlasKey = "atlas";
        valSprite.frameKey = suitAndValueToFrame(aob->playedCardInfo.suit, aob->playedCardInfo.value);
        spriteManAddSprite(valSprite);
    }

    spriteManPopMatrix();

    sprite topBannerSprite = defaultSprite();
    topBannerSprite.pos.x = 88.0f;
    topBannerSprite.pos.y = 0.0f;
    topBannerSprite.atlasKey = "atlas";
    topBannerSprite.frameKey = "top_banner";
    spriteManAddSprite(topBannerSprite);

    spriteManAddText((sprite_text){
        .x = 94.0f,
        .y = 8.0f,
        .text = "player",
        .fontKey = "font"
    });

    spriteManAddText((sprite_text){
        .x = 512.0f,
        .y = 8.0f,
        .text = "enemy",
        .fontKey = "font"
    });

    mat3x3 posMatrix = mat3x3Translate(154.0f, 3.0f);
    spriteManPushMatrix(posMatrix);

    // TODO: maxHealth prop
    f32 healthPercent = aob->playerCardman->hitPoints / 40.0f;
    f32 healthWidth = (1.0f / 22.0f) * healthPercent * 150.0f;

    mat3x3 scaleTransform = mat3x3ScaleXY(healthWidth, 1.0f);
    spriteManPushMatrix(scaleTransform);
    
    sprite healthBarSprite = defaultSprite();
    healthBarSprite.atlasKey = "atlas";
    healthBarSprite.frameKey = "health_square";

    spriteManAddSprite(healthBarSprite);
    spriteManPopMatrix();
    spriteManPopMatrix();

    if (aob->lastHitEnemy != 0) {
        mat3x3 posMatrix = mat3x3Translate(502.0f, 3.0f);
        spriteManPushMatrix(posMatrix);

        f32 healthPercent = aob->lastHitEnemy->hitPoints / 40.0f;
        f32 healthWidth = (1.0f / 22.0f) * healthPercent * 150.0f;

        mat3x3 scaleTransform = mat3x3ScaleXY(-healthWidth, 1.0f);
        spriteManPushMatrix(scaleTransform);
        
        sprite healthBarSprite = defaultSprite();
        healthBarSprite.atlasKey = "atlas";
        healthBarSprite.frameKey = "health_square";

        spriteManAddSprite(healthBarSprite);
        spriteManPopMatrix();
        spriteManPopMatrix();
    }
}

