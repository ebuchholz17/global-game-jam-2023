#include "ace_of_blades.h"

aob_state *aob;

#include "aob_cardman_anims.c"


void activateCardMan (cardman_owner owner) {
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];
        if (!cm->active) {
            zeroMemory((u8 *)cm, sizeof(cardman));

            *cm = (cardman){
                .active = true,
                .owner = owner,

                .suit = CARD_SUIT_SPADE,
                .value = CARD_VAL_ACE,

                .state = CARDMAN_STATE_IDLE,
                //.animState,
                .facing = DIRECTION_DOWN,
                .pos = (vec2){ .x = 240.0f, .y = 120.0f}
                //.vel
            };

            if (owner == CARDMAN_OWNER_PLAYER) {
                aob->playerCardman = cm;
            }

            break;
        }
    }

}

void initAceOfBlades (aob_state* aobState, mem_arena *memory) {
    aob = aobState;

    zeroMemory((u8 *)aob, sizeof(aob_state));

    aob->animations = char_anim_data_ptr_hash_mapInit(memory, 100);
    activateCardMan(CARDMAN_OWNER_PLAYER);
}

void loadAOBHitboxData (char *key, mem_arena *memory) {
    data_asset *hitboxData = getDataAsset(key);

    char_anim_data *animData = (char_anim_data *)allocMemory(memory, sizeof(char_anim_data));
    loadHitboxData(assetMan, hitboxData->data, animData);

    char_anim_data_ptr_hash_mapStore(&aob->animations, animData, key);
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


void updateAceOfBlades (game_input *input, virtual_input *vInput, f32 dt, mem_arena *memory) {
    if (!aob->animationsLoaded) {
        loadAOBHitboxData("cardman_up_idle", memory);
        loadAOBHitboxData("spade_down_idle", memory);
        loadAOBHitboxData("spade_left_idle", memory);
        loadAOBHitboxData("spade_right_idle", memory);
        loadAOBHitboxData("cardman_up_run", memory);
        loadAOBHitboxData("spade_down_run", memory);
        loadAOBHitboxData("spade_left_run", memory);
        loadAOBHitboxData("spade_right_run", memory);
        aob->animationsLoaded = true;
    }

    aob_input aobInput = parseGameInput(input, vInput);

    // update controls/movement
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        if (cm->animState.key != 0) {
            cm->animState.prevKey = tempStringAppend("", cm->animState.key);
        }

        if (cm->owner == CARDMAN_OWNER_PLAYER) {
            switch (cm->state) {
                case CARDMAN_STATE_IDLE: {
                    cm->vel = (vec2){
                        .x = 0.0f,
                        .y = 0.0f
                    };
                    cm->animState.key = getCardmanIdleAnim(cm);
                    if (aobInput.up.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_UP;
                        cm->animState.key = getCardmanWalkingAnim(cm);
                    }
                    if (aobInput.down.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_DOWN;
                        cm->animState.key = getCardmanWalkingAnim(cm);
                    }
                    if (aobInput.left.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_LEFT;
                        cm->animState.key = getCardmanWalkingAnim(cm);
                    }
                    if (aobInput.right.down) {
                        cm->state = CARDMAN_STATE_WALKING;
                        cm->facing = DIRECTION_RIGHT;
                        cm->animState.key = getCardmanWalkingAnim(cm);
                    }
                } break;
                case CARDMAN_STATE_WALKING: {
                    if (!aobInput.up.down &&
                        !aobInput.down.down &&
                        !aobInput.left.down &&
                        !aobInput.right.down)
                    {
                        cm->state = CARDMAN_STATE_IDLE;
                        cm->animState.key = getCardmanIdleAnim(cm);
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
                        cm->animState.key = getCardmanWalkingAnim(cm);
                    }
                } break;
            }
        }

    }

    // update actions
    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        switch (cm->state) {
            case CARDMAN_STATE_WALKING: {
                cm->pos = vec2Add(cm->pos, vec2ScalarMul(dt, cm->vel));
            } break;
        }
        ASSERT(cm->animState.key != 0);
        if (!stringEquals(cm->animState.key, cm->animState.prevKey)) {
            startAnimState(&cm->animState);
        }
        //b32 animationDone = updateAnimState(&cm->animState);
        updateAnimState(&cm->animState);
    }

}

void drawAceOfBlades (plat_api platAPI, f32 gameScale) {
    vec2 cameraOffset = (vec2){ .x = 240.0f, .y = 175.0f };
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


    for (u32 cardmanIndex = 0; cardmanIndex < MAX_NUM_CARDMEN; cardmanIndex++) {
        // TODO: draw player characters on top?
        cardman *cm = &aob->cardmen[cardmanIndex];

        if (!cm->active) {
            continue;
        }

        char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, cm->animState.key);
        char_frame_data *currentFrame = &animData->frames[cm->animState.currentFrame];

        sprite cmSprite = defaultSprite();

        cmSprite.pos.x = cm->pos.x + currentFrame->xOffset;
        cmSprite.pos.y = cm->pos.y + currentFrame->yOffset;
        cmSprite.atlasKey = "atlas";
        cmSprite.frameKey = currentFrame->frameKey;
        ASSERT(cmSprite.frameKey != 0);
        cmSprite.anchor = (vec2){ .x = 0.0f, .y = 1.0f };
        spriteManAddSprite(cmSprite);
    }

    spriteManPopMatrix();
}

