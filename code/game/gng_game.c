#include "gng_game.h"

#include "gng_math.c"
#include "gng_util.c"
#include "gng_random.c"
#include "gng_memory.c"
#include "gng_string.c"
#include "gng_assets.c"
#include "gng_sprites.c"
#include "gng_audio.c"

#include "hitbox/hitbox.c"
#include "ace_of_blades/ace_of_blades.c"

typedef struct key_path_pair {
    char *key;
    char *path;
} key_path_pair;

void loadTextureOnGPU (mem_arena *renderMemory, u32 id, u32 width, u32 height, u8 *pixels) {
    render_cmd_header *header = (render_cmd_header *)allocMemory(renderMemory, sizeof(render_cmd_header));
    header->type = RENDER_CMD_TYPE_LOAD_TEXTURE;

    render_cmd_load_texture *loadTextureCmd = (render_cmd_load_texture *)allocMemory(renderMemory, sizeof(render_cmd_load_texture));
    loadTextureCmd->id = id;
    loadTextureCmd->width = width;
    loadTextureCmd->height = height;
    loadTextureCmd->pixels = pixels;
}

void spriteBatchStart (mem_arena *renderMemory) {
    render_cmd_header *header = (render_cmd_header *)allocMemory(renderMemory, sizeof(render_cmd_header));
    header->type = RENDER_CMD_TYPE_SPRITE_BATCH_START;
}

void spriteBatchEnd (mem_arena *renderMemory) {
    render_cmd_header *header = (render_cmd_header *)allocMemory(renderMemory, sizeof(render_cmd_header));
    header->type = RENDER_CMD_TYPE_SPRITE_BATCH_END;
}

void resetVirtualButton (virtual_button *vButton) {
    vButton->wasDown = vButton->button.down;
    vButton->button.down = false;
    vButton->button.justPressed = false;
}

void setVirtualButton (virtual_button *vButton, b32 touchDown, 
                       f32 touchX, f32 touchY, f32 touchRX, f32 touchRY) 
{
    if (touchDown) {
        vec2 touchCenter = (vec2){ .x = touchX, .y = touchY };
        vec2 centerToCorner = (vec2){ .x = touchRX, .y = touchRY };
        rect touchRect = (rect){
            .min = vec2Subtract(touchCenter, centerToCorner),
            .max = vec2Add(touchCenter, centerToCorner)
        };
        if (rectsIntersect(vButton->boundingBox, touchRect)) {
            if (!vButton->wasDown) {
                vButton->button.justPressed = true;
            }
            vButton->button.down = true;
        }
    }
}

void setVirtualInput (virtual_input *vInput, game_input *input, plat_api *platAPI) {
    f32 gameScale = 1.0f;
    vec2 dpadCenter = (vec2){ 
        .x = gameScale * 90.0f, 
        .y = (f32)platAPI->windowHeight - gameScale * 90.0f 
    };
    vec2 buttonSizeVertical = (vec2){
        .x = 42.0f * gameScale, 
        .y = 54.0f * gameScale
    };
    vec2 buttonSizeHorizontal = (vec2){
        .x = 54.0f * gameScale, 
        .y = 42.0f * gameScale
    };
    f32 dpadButtonOffset = 15.0f;
    vec2 dpadUpCenter = vec2Add(dpadCenter, (vec2){ .y = gameScale * -dpadButtonOffset - buttonSizeVertical.y / 2.0f});
    vInput->dPadUp.boundingBox.min = vec2Subtract(dpadUpCenter, vec2ScalarMul(0.5f, buttonSizeVertical));
    vInput->dPadUp.boundingBox.max = vec2Add(dpadUpCenter, vec2ScalarMul(0.5f, buttonSizeVertical));

    vec2 dpadDownCenter = vec2Add(dpadCenter, (vec2){ .y = gameScale * dpadButtonOffset + buttonSizeVertical.y / 2.0f});
    vInput->dPadDown.boundingBox.min = vec2Subtract(dpadDownCenter, vec2ScalarMul(0.5f, buttonSizeVertical));
    vInput->dPadDown.boundingBox.max = vec2Add(dpadDownCenter, vec2ScalarMul(0.5f, buttonSizeVertical));

    vec2 dpadLeftCenter = vec2Add(dpadCenter, (vec2){ .x = gameScale * -dpadButtonOffset - buttonSizeHorizontal.x / 2.0f});
    vInput->dPadLeft.boundingBox.min = vec2Subtract(dpadLeftCenter, vec2ScalarMul(0.5f, buttonSizeHorizontal));
    vInput->dPadLeft.boundingBox.max = vec2Add(dpadLeftCenter, vec2ScalarMul(0.5f, buttonSizeHorizontal));

    vec2 dpadRightCenter = vec2Add(dpadCenter, (vec2){ .x = gameScale * dpadButtonOffset + buttonSizeHorizontal.x / 2.0f});
    vInput->dPadRight.boundingBox.min = vec2Subtract(dpadRightCenter, vec2ScalarMul(0.5f, buttonSizeHorizontal));
    vInput->dPadRight.boundingBox.max = vec2Add(dpadRightCenter, vec2ScalarMul(0.5f, buttonSizeHorizontal));


    vec2 faceButtonsCenter = (vec2){ 
        .x = (f32)platAPI->windowWidth - gameScale * 90.0f, 
        .y = (f32)platAPI->windowHeight - gameScale * 90.0f 
    };
    vec2 faceButtonSize = (vec2){
        .x = 49.0f * gameScale, 
        .y = 49.0f * gameScale
    };
    f32 faceButtonOffset = 20.0f;
    vec2 topFaceButtonCenter = vec2Add(faceButtonsCenter, (vec2){ .y = gameScale * -faceButtonOffset - faceButtonSize.y / 2.0f});
    vInput->topButton.boundingBox.min = vec2Subtract(topFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));
    vInput->topButton.boundingBox.max = vec2Add(topFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));

    vec2 bottomFaceButtonCenter = vec2Add(faceButtonsCenter, (vec2){ .y = gameScale * faceButtonOffset + faceButtonSize.y / 2.0f});
    vInput->bottomButton.boundingBox.min = vec2Subtract(bottomFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));
    vInput->bottomButton.boundingBox.max = vec2Add(bottomFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));

    vec2 leftFaceButtonCenter = vec2Add(faceButtonsCenter, (vec2){ .x = gameScale * -faceButtonOffset - faceButtonSize.x / 2.0f});
    vInput->leftButton.boundingBox.min = vec2Subtract(leftFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));
    vInput->leftButton.boundingBox.max = vec2Add(leftFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));

    vec2 rightFaceButtonCenter = vec2Add(faceButtonsCenter, (vec2){ .x = gameScale * faceButtonOffset + faceButtonSize.x / 2.0f});
    vInput->rightButton.boundingBox.min = vec2Subtract(rightFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));
    vInput->rightButton.boundingBox.max = vec2Add(rightFaceButtonCenter, vec2ScalarMul(0.5f, faceButtonSize));

    resetVirtualButton(&vInput->dPadUp);
    resetVirtualButton(&vInput->dPadDown);
    resetVirtualButton(&vInput->dPadLeft);
    resetVirtualButton(&vInput->dPadRight);
    resetVirtualButton(&vInput->topButton);
    resetVirtualButton(&vInput->bottomButton);
    resetVirtualButton(&vInput->leftButton);
    resetVirtualButton(&vInput->rightButton);

    for (u32 touchIndex = 0; touchIndex < MAX_NUM_TOUCHES; touchIndex++) {
        touch_input *touch = &input->touches[touchIndex];
        setVirtualButton(&vInput->dPadUp, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->dPadDown, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->dPadLeft, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->dPadRight, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->topButton, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->bottomButton, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->leftButton, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
        setVirtualButton(&vInput->rightButton, touch->touchState.down, touch->x, touch->y, touch->radiusX, touch->radiusY);
    }
}

void resetInput (game_input *input, virtual_input *vInput) {
    input->pointerJustDown = false;

    input->upArrow.justPressed = false;
    input->downArrow.justPressed = false;
    input->leftArrow.justPressed = false;
    input->rightArrow.justPressed = false;

    input->aKey.justPressed = false;
    input->sKey.justPressed = false;
    input->zKey.justPressed = false;
    input->xKey.justPressed = false;

    for (u32 controllerIndex = 0; controllerIndex < MAX_NUM_CONTROLLERS; controllerIndex++) {
        game_controller_input *cont = &input->controllers[controllerIndex];
        if (cont->connected) {
            cont->dPadUp.justPressed = false;
            cont->dPadDown.justPressed = false;
            cont->dPadLeft.justPressed = false;
            cont->dPadRight.justPressed = false;

            cont->start.justPressed = false;
            cont->back.justPressed = false;

            cont->leftStick.justPressed = false;
            cont->rightStick.justPressed = false;

            cont->leftBumper.justPressed = false;
            cont->rightBumper.justPressed = false;

            cont->aButton.justPressed = false;
            cont->bButton.justPressed = false;
            cont->xButton.justPressed = false;
            cont->yButton.justPressed = false;

            cont->leftTriggerButton.justPressed = false;
            cont->rightTriggerButton.justPressed = false;

            cont->leftStickUp.justPressed = false;
            cont->leftStickDown.justPressed = false;
            cont->leftStickLeft.justPressed = false;
            cont->leftStickRight.justPressed = false;

            cont->rightStickUp.justPressed = false;
            cont->rightStickDown.justPressed = false;
            cont->rightStickLeft.justPressed = false;
            cont->rightStickRight.justPressed = false;
        }
    }

    vInput->dPadUp.button.justPressed = false;
    vInput->dPadDown.button.justPressed = false;
    vInput->dPadLeft.button.justPressed = false;
    vInput->dPadRight.button.justPressed = false;
    vInput->topButton.button.justPressed = false;
    vInput->bottomButton.button.justPressed = false;
    vInput->leftButton.button.justPressed = false;
    vInput->rightButton.button.justPressed = false;
}

UPDATE_GNG_GAME(updateGNGGame) {
    gng_game_state *state = (gng_game_state *)platAPI.mainMemory;

    mem_arena scratchMemory = {
        .base = platAPI.scratchMemory,
        .current = platAPI.scratchMemory,
        .capacity = platAPI.scratchMemorySize
    };

    u64 tempStringMemoryCapcity = 1 * 1024 * 1024;
    void *tempStringMemoryBase = allocMemory(&scratchMemory, tempStringMemoryCapcity); 
    tempStringMemory = (mem_arena){
        .base = tempStringMemoryBase,
        .current = tempStringMemoryBase,
        .capacity = tempStringMemoryCapcity
    };

    if (!state->initialized) {
        state->initialized = true;

        setRNGSeed(platAPI.rngSeedFromTime());

        state->memory = (mem_arena){
            .base = platAPI.mainMemory,
            .current = (u8 *)platAPI.mainMemory + sizeof(gng_game_state),
            .capacity = platAPI.mainMemorySize
        };
        initGameAssets(&state->assetMan, &platAPI);

        asset_to_load_list *assetList = &state->assetMan.assetToLoadList;

        // TODO: preprocessed string IDs
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "atlas_data",
            .path = "assets/atlas.txt",
            .type = ASSET_TO_LOAD_TYPE_ATLAS_DATA,
            .loaded = false,
            .key = "atlas"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "atlas_texture",
            .path = "assets/atlas.bmp",
            .type = ASSET_TO_LOAD_TYPE_ATLAS_TEXTURE,
            .loaded = false,
            .key = "atlas"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "guide",
            .path = "assets/guide.bmp",
            .type = ASSET_TO_LOAD_TYPE_BITMAP,
            .loaded = false,
            .key = "guide"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "font",
            .path = "assets/font.bmp",
            .type = ASSET_TO_LOAD_TYPE_BITMAP,
            .loaded = false,
            .key = "font"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "starSFX",
            .path = "assets/star.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "starSFX"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "attack",
            .path = "assets/attack.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "attack"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "card_flip",
            .path = "assets/card_flip.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "card_flip"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "dash",
            .path = "assets/dash.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "dash"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "impact",
            .path = "assets/impact.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "impact"
        });
        asset_to_load_listPush(assetList, (asset_to_load){
            .name = "level_up",
            .path = "assets/level_up.wav",
            .type = ASSET_TO_LOAD_TYPE_WAV,
            .loaded = false,
            .key = "level_up"
        });

        key_path_pair hitboxFiles[] = {
            {.key = "cardman_up_idle", .path = "assets/hitbox/cardman_up_idle.txt" },
            {.key = "cardman_up_run", .path = "assets/hitbox/cardman_up_run.txt" },
            {.key = "spade_down_idle", .path = "assets/hitbox/spade_down_idle.txt" },
            {.key = "spade_down_run", .path = "assets/hitbox/spade_down_run.txt" },
            {.key = "spade_left_idle", .path = "assets/hitbox/spade_left_idle.txt" },
            {.key = "spade_left_run", .path = "assets/hitbox/spade_left_run.txt" },
            {.key = "spade_right_idle", .path = "assets/hitbox/spade_right_idle.txt" },
            {.key = "spade_right_run", .path = "assets/hitbox/spade_right_run.txt" },
            {.key = "club_down_idle", .path = "assets/hitbox/club_down_idle.txt" },
            {.key = "club_down_run", .path = "assets/hitbox/club_down_run.txt" },
            {.key = "club_left_idle", .path = "assets/hitbox/club_left_idle.txt" },
            {.key = "club_left_run", .path = "assets/hitbox/club_left_run.txt" },
            {.key = "club_right_idle", .path = "assets/hitbox/club_right_idle.txt" },
            {.key = "club_right_run", .path = "assets/hitbox/club_right_run.txt" },
            {.key = "diamond_down_idle", .path = "assets/hitbox/diamond_down_idle.txt" },
            {.key = "diamond_down_run", .path = "assets/hitbox/diamond_down_run.txt" },
            {.key = "diamond_left_idle", .path = "assets/hitbox/diamond_left_idle.txt" },
            {.key = "diamond_left_run", .path = "assets/hitbox/diamond_left_run.txt" },
            {.key = "diamond_right_idle", .path = "assets/hitbox/diamond_right_idle.txt" },
            {.key = "diamond_right_run", .path = "assets/hitbox/diamond_right_run.txt" },
            {.key = "heart_down_idle", .path = "assets/hitbox/heart_down_idle.txt" },
            {.key = "heart_down_run", .path = "assets/hitbox/heart_down_run.txt" },
            {.key = "heart_left_idle", .path = "assets/hitbox/heart_left_idle.txt" },
            {.key = "heart_left_run", .path = "assets/hitbox/heart_left_run.txt" },
            {.key = "heart_right_idle", .path = "assets/hitbox/heart_right_idle.txt" },
            {.key = "heart_right_run", .path = "assets/hitbox/heart_right_run.txt" },


            {.key = "spade_left_hitstun", .path = "assets/hitbox/spade_left_hitstun.txt" },
            {.key = "spade_right_hitstun", .path = "assets/hitbox/spade_right_hitstun.txt" },
            {.key = "club_left_hitstun", .path = "assets/hitbox/club_left_hitstun.txt" },
            {.key = "club_right_hitstun", .path = "assets/hitbox/club_right_hitstun.txt" },
            {.key = "diamond_left_hitstun", .path = "assets/hitbox/diamond_left_hitstun.txt" },
            {.key = "diamond_right_hitstun", .path = "assets/hitbox/diamond_right_hitstun.txt" },
            {.key = "heart_left_hitstun", .path = "assets/hitbox/heart_left_hitstun.txt" },
            {.key = "heart_right_hitstun", .path = "assets/hitbox/heart_right_hitstun.txt" },
            {.key = "joker_left_hitstun", .path = "assets/hitbox/joker_left_hitstun.txt" },
            {.key = "joker_right_hitstun", .path = "assets/hitbox/joker_right_hitstun.txt" },

            {.key = "cardman_up_katana_0", .path = "assets/hitbox/cardman_up_katana_0.txt" },
            {.key = "spade_down_katana_0", .path = "assets/hitbox/spade_down_katana_0.txt" },
            {.key = "spade_left_katana_0", .path = "assets/hitbox/spade_left_katana_0.txt" },
            {.key = "spade_right_katana_0", .path = "assets/hitbox/spade_right_katana_0.txt" },


            {.key = "spade_play_card_0", .path = "assets/hitbox/spade_play_card_0.txt" },
            {.key = "spade_play_card_1", .path = "assets/hitbox/spade_play_card_1.txt" },
            {.key = "club_play_card_0", .path = "assets/hitbox/club_play_card_0.txt" },
            {.key = "club_play_card_1", .path = "assets/hitbox/club_play_card_1.txt" },
            {.key = "diamond_play_card_0", .path = "assets/hitbox/diamond_play_card_0.txt" },
            {.key = "diamond_play_card_1", .path = "assets/hitbox/diamond_play_card_1.txt" },
            {.key = "heart_play_card_0", .path = "assets/hitbox/heart_play_card_0.txt" },
            {.key = "heart_play_card_1", .path = "assets/hitbox/heart_play_card_1.txt" },

            {.key = "spade_left_dash", .path = "assets/hitbox/spade_left_dash.txt" },
            {.key = "spade_right_dash", .path = "assets/hitbox/spade_right_dash.txt" },
            {.key = "spade_down_dash", .path = "assets/hitbox/spade_down_dash.txt" },
            {.key = "spade_left_backstep", .path = "assets/hitbox/spade_left_backstep.txt" },
            {.key = "spade_right_backstep", .path = "assets/hitbox/spade_right_backstep.txt" },
            {.key = "spade_down_backstep", .path = "assets/hitbox/spade_down_backstep.txt" },

            {.key = "club_left_dash", .path = "assets/hitbox/club_left_dash.txt" },
            {.key = "club_right_dash", .path = "assets/hitbox/club_right_dash.txt" },
            {.key = "club_down_dash", .path = "assets/hitbox/club_down_dash.txt" },
            {.key = "club_left_backstep", .path = "assets/hitbox/club_left_backstep.txt" },
            {.key = "club_right_backstep", .path = "assets/hitbox/club_right_backstep.txt" },
            {.key = "club_down_backstep", .path = "assets/hitbox/club_down_backstep.txt" },

            {.key = "heart_left_dash", .path = "assets/hitbox/heart_left_dash.txt" },
            {.key = "heart_right_dash", .path = "assets/hitbox/heart_right_dash.txt" },
            {.key = "heart_down_dash", .path = "assets/hitbox/heart_down_dash.txt" },
            {.key = "heart_left_backstep", .path = "assets/hitbox/heart_left_backstep.txt" },
            {.key = "heart_right_backstep", .path = "assets/hitbox/heart_right_backstep.txt" },
            {.key = "heart_down_backstep", .path = "assets/hitbox/heart_down_backstep.txt" },

            {.key = "diamond_left_dash", .path = "assets/hitbox/diamond_left_dash.txt" },
            {.key = "diamond_right_dash", .path = "assets/hitbox/diamond_right_dash.txt" },
            {.key = "diamond_down_dash", .path = "assets/hitbox/diamond_down_dash.txt" },
            {.key = "diamond_left_backstep", .path = "assets/hitbox/diamond_left_backstep.txt" },
            {.key = "diamond_right_backstep", .path = "assets/hitbox/diamond_right_backstep.txt" },
            {.key = "diamond_down_backstep", .path = "assets/hitbox/diamond_down_backstep.txt" },

            {.key = "cardman_up_dash", .path = "assets/hitbox/cardman_up_dash.txt" },
            {.key = "cardman_up_backstep", .path = "assets/hitbox/cardman_up_backstep.txt" },

            {.key = "cardman_up_katana_1", .path = "assets/hitbox/cardman_up_katana_1.txt" },
            {.key = "cardman_up_katana_2", .path = "assets/hitbox/cardman_up_katana_2.txt" },
            {.key = "cardman_up_punch_0", .path = "assets/hitbox/cardman_up_punch_0.txt" },
            {.key = "cardman_up_punch_1", .path = "assets/hitbox/cardman_up_punch_1.txt" },
            {.key = "cardman_up_spear_0", .path = "assets/hitbox/cardman_up_spear_0.txt" },
            {.key = "cardman_up_spear_1", .path = "assets/hitbox/cardman_up_spear_1.txt" },
            {.key = "cardman_up_wand", .path = "assets/hitbox/cardman_up_wand.txt" },
            {.key = "club_down_punch_0", .path = "assets/hitbox/club_down_punch_0.txt" },
            {.key = "club_down_punch_1", .path = "assets/hitbox/club_down_punch_1.txt" },
            {.key = "club_left_punch_0", .path = "assets/hitbox/club_left_punch_0.txt" },
            {.key = "club_left_punch_1", .path = "assets/hitbox/club_left_punch_1.txt" },
            {.key = "club_right_punch_0", .path = "assets/hitbox/club_right_punch_0.txt" },
            {.key = "club_right_punch_1", .path = "assets/hitbox/club_right_punch_1.txt" },
            {.key = "diamond_down_wand", .path = "assets/hitbox/diamond_down_wand.txt" },
            {.key = "diamond_left_wand", .path = "assets/hitbox/diamond_left_wand.txt" },
            {.key = "diamond_right_wand", .path = "assets/hitbox/diamond_right_wand.txt" },
            {.key = "heart_down_spear_0", .path = "assets/hitbox/heart_down_spear_0.txt" },
            {.key = "heart_down_spear_1", .path = "assets/hitbox/heart_down_spear_1.txt" },
            {.key = "heart_left_spear_0", .path = "assets/hitbox/heart_left_spear_0.txt" },
            {.key = "heart_left_spear_1", .path = "assets/hitbox/heart_left_spear_1.txt" },
            {.key = "heart_right_spear_0", .path = "assets/hitbox/heart_right_spear_0.txt" },
            {.key = "heart_right_spear_1", .path = "assets/hitbox/heart_right_spear_1.txt" },
            {.key = "heart_spear_spin", .path = "assets/hitbox/heart_spear_spin.txt" },
            {.key = "spade_down_katana_1", .path = "assets/hitbox/spade_down_katana_1.txt" },
            {.key = "spade_down_katana_2", .path = "assets/hitbox/spade_down_katana_2.txt" },
            {.key = "spade_left_katana_1", .path = "assets/hitbox/spade_left_katana_1.txt" },
            {.key = "spade_left_katana_2", .path = "assets/hitbox/spade_left_katana_2.txt" },
            {.key = "spade_right_katana_1", .path = "assets/hitbox/spade_right_katana_1.txt" },
            {.key = "spade_right_katana_2", .path = "assets/hitbox/spade_right_katana_2.txt" },
            {.key = "star", .path = "assets/hitbox/star.txt" },

            {.key = "joker_down_idle", .path = "assets/hitbox/joker_down_idle.txt" },
            {.key = "joker_down_run", .path = "assets/hitbox/joker_down_run.txt" },
            {.key = "joker_left_idle", .path = "assets/hitbox/joker_left_idle.txt" },
            {.key = "joker_left_run", .path = "assets/hitbox/joker_left_run.txt" },
            {.key = "joker_right_idle", .path = "assets/hitbox/joker_right_idle.txt" },
            {.key = "joker_right_run", .path = "assets/hitbox/joker_right_run.txt" },
            {.key = "joker_play_card_0", .path = "assets/hitbox/joker_play_card_0.txt" },
            {.key = "joker_play_card_1", .path = "assets/hitbox/joker_play_card_1.txt" },

        };
        u32 numHitboxFiles = sizeof(hitboxFiles) / sizeof(key_path_pair);
        for (u32 hitboxIndex = 0; hitboxIndex < numHitboxFiles; hitboxIndex++) {
            key_path_pair *kpPair = &hitboxFiles[hitboxIndex];
            asset_to_load_listPush(assetList, (asset_to_load){
                .name = kpPair->key,
                .path = kpPair->path,
                .type = ASSET_TO_LOAD_TYPE_DATA,
                .loaded = false,
                .key = kpPair->key
            });
        }


        for (u32 assetIndex = 0; assetIndex < assetList->numValues; ++assetIndex) {
            asset_to_load *asset = &assetList->values[assetIndex];
            platAPI.loadFile(asset->name, asset->path);
        }

        spriteManInit(&state->spriteMan, &state->assetMan);
        soundManInit(&state->soundMan, &state->assetMan);

        platAPI.consoleLog("game ready");

    }

    spriteMan->sprites = sprite_listInit(&scratchMemory, SPRITE_LIST_MAX_NUM_SPRITES);
    spriteMan->matrixStackIndex = 0;
    spriteMan->matrixStack[0] = mat3x3Identity();

    asset_man *assetMan = &state->assetMan;
    if (numLoadedFiles > 0) {
        for (u32 loadedFileIndex = 0; loadedFileIndex < numLoadedFiles; loadedFileIndex++) {
            plat_loaded_file *file = &loadedFiles[loadedFileIndex];

            for (u32 listIndex = 0; listIndex < state->assetMan.assetToLoadList.numValues; listIndex++) {
                asset_to_load *asset = &assetMan->assetToLoadList.values[listIndex];
                if (stringEquals(asset->name, file->id)) {
                    switch (asset->type) {
                        case ASSET_TO_LOAD_TYPE_BITMAP: {
                            parseBitmap(assetMan, asset->key, file->data);
                            texture_asset *textureAsset = texture_asset_hash_mapGetPtr(&assetMan->textures, asset->key);
                            loadTextureOnGPU(renderMemory, textureAsset->id, textureAsset->width, textureAsset->height, textureAsset->pixels);
                        } break;
                        case ASSET_TO_LOAD_TYPE_ATLAS_TEXTURE: {
                            atlas_asset *atlas = atlas_asset_hash_mapGetPtr(&assetMan->atlases, asset->key);

                            if (atlas == 0) {
                                atlas_asset_hash_mapStore(&assetMan->atlases, (atlas_asset){0}, asset->key);
                                atlas = atlas_asset_hash_mapGetPtr(&assetMan->atlases, asset->key);
                            }

                            parseBitmap(assetMan, asset->key, file->data);
                            texture_asset *textureAsset = texture_asset_hash_mapGetPtr(&assetMan->textures, asset->key);
                            loadTextureOnGPU(renderMemory, textureAsset->id, textureAsset->width, textureAsset->height, textureAsset->pixels);
                            atlas->textureID = textureAsset->id;
                        } break;
                        case ASSET_TO_LOAD_TYPE_ATLAS_DATA: {
                            atlas_asset *atlas = atlas_asset_hash_mapGetPtr(&assetMan->atlases, asset->key);

                            if (atlas == 0) {
                                atlas_asset_hash_mapStore(&assetMan->atlases, (atlas_asset){0}, asset->key);
                                atlas = atlas_asset_hash_mapGetPtr(&assetMan->atlases, asset->key);
                            }

                            parseAtlasData(assetMan, atlas, file->data);
                        } break;
                        case ASSET_TO_LOAD_TYPE_WAV: {
                            parseWav(assetMan, asset->key, file->data, platAPI);
                        } break;
                        case ASSET_TO_LOAD_TYPE_DATA: {
                            loadDataFile(assetMan, asset->key, file->data, file->dataSize);
                        } break;
                        default: {
                            ASSERT(0);
                        } break;
                    }
                    platAPI.consoleLog("file with name loaded:");
                    platAPI.consoleLog(asset->name);
                    asset->loaded = true;
                    break;
                }
            }
        }
    }

    if (!assetMan->allFilesLoaded) {
        b32 allLoaded = true;
        for (u32 assetIndex = 0; assetIndex < state->assetMan.assetToLoadList.numValues; ++assetIndex) {
            asset_to_load *asset = &assetMan->assetToLoadList.values[assetIndex];
            if (!asset->loaded) {
                allLoaded = false;
                break;
            }
        }
        state->assetMan.allFilesLoaded = allLoaded;
    }

    f32 screenWidth = (f32)platAPI.windowWidth;
    f32 screenHeight = (f32)platAPI.windowHeight;

    f32 gameWidth = 640.0f;
    f32 gameHeight = 360.0f;
    f32 normalAspectRatio = gameWidth / gameHeight;

    f32 actualAspectRatio = screenWidth / screenHeight;
    f32 gameScale;
    vec2 gameOrigin;
    if (actualAspectRatio < normalAspectRatio) {
        f32 widthRatio = screenWidth / gameWidth;
        gameScale = widthRatio;
        //gameScale *= 0.5f;
        gameOrigin.x = (screenWidth - (gameWidth * gameScale)) / 2.0f;
        gameOrigin.y = (screenHeight - (gameHeight * gameScale)) / 2.0f;
    }
    else {
        float heightRatio = screenHeight / gameHeight;
        gameScale = heightRatio;
        //gameScale *= 0.5f;
        gameOrigin.x = (screenWidth - (gameWidth * gameScale)) / 2.0f;
        gameOrigin.y = (screenHeight - (gameHeight * gameScale)) / 2.0f;
    }





    if (state->assetMan.allFilesLoaded) {

        if (!state->aobState.initialized) {
            initAceOfBlades(&state->aobState, &state->memory);
        }

        if (platAPI.hasTouchControls) {
            setVirtualInput(&state->vInput, input, &platAPI);
        }

        state->t += dt;
        while (state->t > 1000.0f) {
            state->t -= 1000.0f;
        }
        f32 remainingTime = dt;
        f32 updateDelta = (1.0f / 144.0f);
        while (remainingTime > 0) {
            f32 timeStep;
            if (remainingTime >= updateDelta) {
                timeStep = updateDelta;
            }
            else {
                timeStep = remainingTime;
            }
            remainingTime -= updateDelta;
            
            // update
            updateAceOfBlades(input, &state->vInput, timeStep, &state->memory, &scratchMemory, platAPI);

            resetInput(input, &state->vInput);
        }

        if (!(platAPI.windowWidth == 0 && platAPI.windowHeight ==0)) {
            spriteManPushTransform((sprite_transform){ .pos = gameOrigin, .scale = gameScale });

            // draw
            drawAceOfBlades(platAPI, gameScale, &scratchMemory);

            spriteManPopMatrix();

            if (platAPI.hasTouchControls) {

                virtual_input *vInput = &state->vInput;
                sprite testButtonSprite = defaultSprite();
                testButtonSprite.atlasKey = "atlas";
                testButtonSprite.scale = 1.0f;
                testButtonSprite.alpha = 0.5f;

                if (vInput->dPadUp.button.down) {
                    testButtonSprite.frameKey = "dpad_up_down";
                }
                else {
                    testButtonSprite.frameKey = "dpad_up_up";
                }
                testButtonSprite.pos.x = vInput->dPadUp.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->dPadUp.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);
                
                if (vInput->dPadDown.button.down) {
                    testButtonSprite.frameKey = "dpad_down_down";
                }
                else {
                    testButtonSprite.frameKey = "dpad_down_up";
                }
                testButtonSprite.pos.x = vInput->dPadDown.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->dPadDown.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);

                if (vInput->dPadLeft.button.down) {
                    testButtonSprite.frameKey = "dpad_left_down";
                }
                else {
                    testButtonSprite.frameKey = "dpad_left_up";
                }
                testButtonSprite.pos.x = vInput->dPadLeft.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->dPadLeft.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);

                if (vInput->dPadRight.button.down) {
                    testButtonSprite.frameKey = "dpad_right_down";
                }
                else {
                    testButtonSprite.frameKey = "dpad_right_up";
                }
                testButtonSprite.pos.x = vInput->dPadRight.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->dPadRight.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);

                if (vInput->topButton.button.down) {
                    testButtonSprite.frameKey = "face_button_down";
                }
                else {
                    testButtonSprite.frameKey = "face_button_up";
                }
                testButtonSprite.pos.x = vInput->topButton.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->topButton.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);
                
                if (vInput->bottomButton.button.down) {
                    testButtonSprite.frameKey = "face_button_down";
                }
                else {
                    testButtonSprite.frameKey = "face_button_up";
                }
                testButtonSprite.pos.x = vInput->bottomButton.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->bottomButton.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);

                if (vInput->leftButton.button.down) {
                    testButtonSprite.frameKey = "face_button_down";
                }
                else {
                    testButtonSprite.frameKey = "face_button_up";
                }
                testButtonSprite.pos.x = vInput->leftButton.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->leftButton.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);

                if (vInput->rightButton.button.down) {
                    testButtonSprite.frameKey = "face_button_down";
                }
                else {
                    testButtonSprite.frameKey = "face_button_up";
                }
                testButtonSprite.pos.x = vInput->rightButton.boundingBox.min.x;
                testButtonSprite.pos.y = vInput->rightButton.boundingBox.min.y;
                spriteManAddSprite(testButtonSprite);


            }
    }


    spriteBatchStart(renderMemory);


    render_cmd_header *header = (render_cmd_header *)allocMemory(renderMemory, sizeof(render_cmd_header));
    header->type = RENDER_CMD_TYPE_SPRITE_BATCH_DRAW;


    render_cmd_sprite_batch_draw *cmd = (render_cmd_sprite_batch_draw *)allocMemory(renderMemory, sizeof(render_cmd_sprite_batch_draw));
    cmd->numSprites = spriteMan->sprites.numValues;


    render_cmd_sprite_data *spriteDatas = (render_cmd_sprite_data *)allocMemory(renderMemory, cmd->numSprites * sizeof(render_cmd_sprite_data));
    cmd->sprites = spriteDatas;


    for (u32 spriteIndex = 0; spriteIndex < spriteMan->sprites.numValues; spriteIndex++) {
        sprite *s = &spriteMan->sprites.values[spriteIndex];

        mat3x3 spriteTransform = mat3x3Identity();
        spriteTransform.m[2] = -s->anchor.x;
        spriteTransform.m[5] = -s->anchor.y;

        f32 scaledWidth = s->scale * s->width;
        f32 scaledHeight = s->scale * s->height;

        mat3x3 scaleMatrix = mat3x3ScaleXY(scaledWidth, scaledHeight);
        spriteTransform = mat3x3MatrixMul(scaleMatrix, spriteTransform);
        spriteTransform = mat3x3MatrixMul(mat3x3Rotate2PI(s->rotation), spriteTransform);
        spriteTransform = mat3x3MatrixMul(mat3x3Translate(s->pos.x, s->pos.y), spriteTransform);

        spriteTransform = mat3x3MatrixMul(s->parentTransform, spriteTransform);

        render_cmd_sprite_data *spriteData = &spriteDatas[spriteIndex];

        vec2 pos00 = vec2Mat3x3Mul(spriteTransform, (vec2){ .x = 0.0f, .y = 0.0f });
        spriteData->positions[0] = pos00.x;
        spriteData->positions[1] = pos00.y;
        spriteData->positions[2] = 1.0f;

        vec2 pos01 = vec2Mat3x3Mul(spriteTransform, (vec2){ .x = 1.0f, .y = 0.0f });
        spriteData->positions[3] = pos01.x;
        spriteData->positions[4] = pos01.y;
        spriteData->positions[5] = 1.0f;

        vec2 pos10 = vec2Mat3x3Mul(spriteTransform, (vec2){ .x = 0.0f, .y = 1.0f });
        spriteData->positions[6] = pos10.x;
        spriteData->positions[7] = pos10.y;
        spriteData->positions[8] = 1.0f;

        vec2 pos11 = vec2Mat3x3Mul(spriteTransform, (vec2){ .x = 1.0f, .y = 1.0f });
        spriteData->positions[9]  = pos11.x;
        spriteData->positions[10] = pos11.y;
        spriteData->positions[11] = 1.0f;

        spriteData->texCoords[0] = s->frameCorners[0].x;
        spriteData->texCoords[1] = s->frameCorners[0].y;
        spriteData->texCoords[2] = s->frameCorners[1].x;
        spriteData->texCoords[3] = s->frameCorners[1].y;
        spriteData->texCoords[4] = s->frameCorners[2].x;
        spriteData->texCoords[5] = s->frameCorners[2].y;
        spriteData->texCoords[6] = s->frameCorners[3].x;
        spriteData->texCoords[7] = s->frameCorners[3].y;

        f32 red = (f32)((s->tint >> 16) & (0xff)) / 255.0f;
        f32 green = (f32)((s->tint >> 8) & (0xff)) / 255.0f;
        f32 blue = (f32)((s->tint) & (0xff)) / 255.0f;

        spriteData->colors[0]  = red;
        spriteData->colors[1]  = blue;
        spriteData->colors[2]  = green;
        spriteData->colors[3]  = s->alpha;
        spriteData->colors[4]  = red;
        spriteData->colors[5]  = blue;
        spriteData->colors[6]  = green;
        spriteData->colors[7]  = s->alpha;
        spriteData->colors[8]  = red;
        spriteData->colors[9]  = blue;
        spriteData->colors[10] = green;
        spriteData->colors[11] = s->alpha;
        spriteData->colors[12] = red;
        spriteData->colors[13] = blue;
        spriteData->colors[14] = green;
        spriteData->colors[15] = s->alpha;

        spriteData->textureID = s->textureID;
    }

    spriteBatchEnd(renderMemory);
        }

}

GET_SOUND_SAMPLES_GNG_GAME(getSoundSamplesGNGGame) {
    gng_game_state *state = (gng_game_state *)platAPI.mainMemory;

    sound_man *soundMan = &state->soundMan;
    asset_man *assetMan = &state->assetMan;

    float volume = 0.25f;

    // TODO: bgm

    for (u32 i = 0; i < gameSoundOutput->sampleCount; i++) {
        sound_sample *currentSample = &gameSoundOutput->samples[i];

        float sampleValue = 0.0f;

        for (int soundIndex = 0; soundIndex < SOUND_MAN_MAX_PLAYING_SOUNDS; soundIndex++) {
            playing_sound *sound = &soundMan->playingSounds[soundIndex];
            if (sound->active) {
                sound_asset *soundAsset = getSound(assetMan, sound->key);

                i16 soundValue = soundAsset->samples[sound->currentSampleIndex];
                f32 floatValue = (f32)soundValue / 32767;
                sampleValue += floatValue;

                sound->currentSampleIndex++;
                if (sound->currentSampleIndex >= soundAsset->numSamples) {
                    sound->active = false;
                }
            }
        }

        sampleValue = sampleValue > 1.0f ? 1.0f : sampleValue;
        sampleValue = sampleValue < -1.0f ? -1.0f : sampleValue;
        currentSample->value = volume * sampleValue;
    }
}
