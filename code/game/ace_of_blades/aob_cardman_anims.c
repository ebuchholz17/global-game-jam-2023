#include "aob_cardman_anims.h"

char *getPermStringForTempAnimKey (char *key) {
    char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, key);
    ASSERT(animData != 0);
    return animData->key;
}

char *getCardmanIdleAnim (cardman *cm) {
    if (cm->facing == DIRECTION_UP) {
        return "cardman_up_idle";
    }
    else {
        char *suit = 0;
        switch (cm->suit) {
            case CARD_SUIT_SPADE: { 
                suit = "spade_";
            } break;
            case CARD_SUIT_CLUB: { 
                suit = "club_";
            } break;
            case CARD_SUIT_DIAMOND: { 
                suit = "diamond_";
            } break;
            case CARD_SUIT_HEART: { 
                suit = "heart_";
            } break;
        }
        ASSERT(suit != 0);

        char *dir = 0;
        switch (cm->facing) {
            case DIRECTION_UP: { 
                dir = "up_";
            } break;
            case DIRECTION_DOWN: { 
                dir = "down_";
            } break;
            case DIRECTION_LEFT: { 
                dir = "left_";
            } break;
            case DIRECTION_RIGHT: { 
                dir = "right_";
            } break;
        }
        ASSERT(dir != 0);

        char *result = tempStringAppend(suit, dir);
        return getPermStringForTempAnimKey(tempStringAppend(result, "idle"));
    }
}

char *getCardmanPlayCardStartAnim (cardman *cm) {
    char *suit = 0;
    switch (cm->suit) {
        case CARD_SUIT_SPADE: { 
            suit = "spade_";
        } break;
        case CARD_SUIT_CLUB: { 
            suit = "club_";
        } break;
        case CARD_SUIT_DIAMOND: { 
            suit = "diamond_";
        } break;
        case CARD_SUIT_HEART: { 
            suit = "heart_";
        } break;
    }
    ASSERT(suit != 0);

    char *result = tempStringAppend(suit, "play_card_0");
    return getPermStringForTempAnimKey(result);
}

char *getCardmanPlayCardEndAnim (cardman *cm) {
    char *suit = 0;
    switch (cm->suit) {
        case CARD_SUIT_SPADE: { 
            suit = "spade_";
        } break;
        case CARD_SUIT_CLUB: { 
            suit = "club_";
        } break;
        case CARD_SUIT_DIAMOND: { 
            suit = "diamond_";
        } break;
        case CARD_SUIT_HEART: { 
            suit = "heart_";
        } break;
    }
    ASSERT(suit != 0);

    char *result = tempStringAppend(suit, "play_card_1");
    return getPermStringForTempAnimKey(result);
}

char *getCardSuitFrame (card_suit suit) {
    switch (suit) {
        case CARD_SUIT_SPADE: { 
            return "spade_card";
        } break;
        case CARD_SUIT_CLUB: { 
            return "club_card";
        } break;
        case CARD_SUIT_DIAMOND: { 
            return "diamond_card";
        } break;
        case CARD_SUIT_HEART: { 
            return "heart_card";
        } break;
    }
    return "spade_card";
}

char *getCardmanWalkingAnim (cardman *cm) {
    if (cm->facing == DIRECTION_UP) {
        return "cardman_up_run";
    }
    else {
        char *suit = 0;
        switch (cm->suit) {
            case CARD_SUIT_SPADE: { 
                suit = "spade_";
            } break;
            case CARD_SUIT_CLUB: { 
                suit = "club_";
            } break;
            case CARD_SUIT_DIAMOND: { 
                suit = "diamond_";
            } break;
            case CARD_SUIT_HEART: { 
                suit = "heart_";
            } break;
        }
        ASSERT(suit != 0);

        char *dir = 0;
        switch (cm->facing) {
            case DIRECTION_UP: { 
                dir = "up_";
            } break;
            case DIRECTION_DOWN: { 
                dir = "down_";
            } break;
            case DIRECTION_LEFT: { 
                dir = "left_";
            } break;
            case DIRECTION_RIGHT: { 
                dir = "right_";
            } break;
        }
        ASSERT(dir != 0);

        char *result = tempStringAppend(suit, dir);
        return getPermStringForTempAnimKey(tempStringAppend(result, "run"));
    }
}

char *getCardmanHitstunAnim (cardman *cm) {
    char *suit = 0;
    switch (cm->suit) {
        case CARD_SUIT_SPADE: { 
            suit = "spade_";
        } break;
        case CARD_SUIT_CLUB: { 
            suit = "club_";
        } break;
        case CARD_SUIT_DIAMOND: { 
            suit = "diamond_";
        } break;
        case CARD_SUIT_HEART: { 
            suit = "heart_";
        } break;
    }
    ASSERT(suit != 0);

    char *dir = 0;
    switch (cm->facing) {
        // use left/right animations even when facing up/down
        case DIRECTION_UP: { 
            dir = "right_";
        } break;
        case DIRECTION_DOWN: { 
            dir = "right_";
        } break;
        case DIRECTION_LEFT: { 
            dir = "left_";
        } break;
        case DIRECTION_RIGHT: { 
            dir = "right_";
        } break;
    }
    ASSERT(dir != 0);

    char *result = tempStringAppend(suit, dir);
    return getPermStringForTempAnimKey(tempStringAppend(result, "hitstun"));
}


direction getCardmanFacingForMoveDir (cardman *cm, vec2 moveDir) {
    direction dir = DIRECTION_DOWN;
    if (moveDir.y == 0.0f) {
        if (moveDir.x < 0.0f) {
            dir = DIRECTION_LEFT;
        }
        else {
            dir = DIRECTION_RIGHT;
        }
    }
    else if (moveDir.x == 0.0f) {
        if (moveDir.y < 0.0f) {
            dir = DIRECTION_UP;
        }
        else {
            dir = DIRECTION_DOWN;
        }
    }
    else {
        if (moveDir.x < 0.0f && moveDir.y < 0.0f) {
            if (cm->facing != DIRECTION_UP && cm->facing != DIRECTION_LEFT) {
                dir = DIRECTION_LEFT;
            }
            else {
                dir = cm->facing;
            }
        }
        if (moveDir.x >= 0.0f && moveDir.y < 0.0f) {
            if (cm->facing != DIRECTION_UP && cm->facing != DIRECTION_RIGHT) {
                dir = DIRECTION_RIGHT;
            }
            else {
                dir = cm->facing;
            }
        }
        if (moveDir.x < 0.0f && moveDir.y >= 0.0f) {
            if (cm->facing != DIRECTION_DOWN && cm->facing != DIRECTION_LEFT) {
                dir = DIRECTION_LEFT;
            }
            else {
                dir = cm->facing;
            }
        }
        if (moveDir.x >= 0.0f && moveDir.y >= 0.0f) {
            if (cm->facing != DIRECTION_DOWN && cm->facing != DIRECTION_RIGHT) {
                dir = DIRECTION_RIGHT;
            }
            else {
                dir = cm->facing;
            }
        }
    }
    return dir;
}

direction getCPUCardmanFacingForMoveDir (cardman *cm, vec2 moveDir) {
    f32 upRightDot = vec2Dot((vec2){ .x = 1.0f, .y = -1.0f }, moveDir);
    f32 downRightDot = vec2Dot((vec2){ .x = 1.0f, .y = 1.0f }, moveDir);

    if (upRightDot >= 0.0f && downRightDot >= 0.0f) {
        return DIRECTION_RIGHT;
    }
    else if (upRightDot < 0.0f && downRightDot >= 0.0f) {
        return DIRECTION_DOWN;
    }
    else if (upRightDot < 0.0f && downRightDot < 0.0f) {
        return DIRECTION_LEFT;
    }
    else {
        return DIRECTION_UP;
    }
}

void startAnimState (animation_state *animState) {
    animState->currentFrameStep = 0;
    animState->currentFrame = 0;
    animState->totalFrames = 0;
    animState->prevKey = animState->key;
}

b32 updateAnimState (animation_state *animState, f32 dt) {
    char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, animState->key);
    char_frame_data *currentFrame = &animData->frames[animState->currentFrame];

    b32 animationComplete = false;

    f32 timePerFrame = 1.0f / 144.0f;

    u32 startFrame = animState->currentFrame;

    animState->t += dt * animState->speedMultiplier;
    while (animState->t >= timePerFrame) {
        animState->t -= timePerFrame;
        ++animState->currentFrameStep;
        ++animState->totalFrames;

        if (animState->currentFrameStep >= currentFrame->duration) {
            animState->currentFrameStep = 0;
            ++animState->currentFrame;
            if (animState->currentFrame == animData->numFrames) {
                animState->currentFrame = 0;
                animationComplete = true;
            }
            // avoid skipping over hitbox frames
            if (animState->currentFrame - startFrame > 1) {
                break;
            }
        }
    }

    return animationComplete;
}

char *suitAndValueToFrame (card_suit suit, card_val value) {
    char *color;
    switch (suit) {
        case CARD_SUIT_SPADE:
        case CARD_SUIT_CLUB: {
            color = "black_";
        } break;
        case CARD_SUIT_DIAMOND:
        case CARD_SUIT_HEART: {
            color = "red_";
        } break;
    }

    char *val;
    switch (value) {
        case CARD_VAL_ACE: {
            val = "a";
        } break;
        case CARD_VAL_TWO: {
            val = "2";
        } break;
        case CARD_VAL_THREE: {
            val = "3";
        } break;
        case CARD_VAL_FOUR: {
            val = "4";
        } break;
        case CARD_VAL_FIVE: {
            val = "5";
        } break;
        case CARD_VAL_SIX: {
            val = "6";
        } break;
        case CARD_VAL_SEVEN: {
            val = "7";
        } break;
        case CARD_VAL_EIGHT: {
            val = "8";
        } break;
        case CARD_VAL_NINE: {
            val = "9";
        } break;
        case CARD_VAL_TEN: {
            val = "10";
        } break;
        case CARD_VAL_JACK: {
            val = "j";
        } break;
        case CARD_VAL_QUEEN: {
            val = "q";
        } break;
        case CARD_VAL_KING: {
            val = "k";
        } break;
    }

    return tempStringAppend(color, val);
}

char *getCardmanKatanaAnim (cardman *cm) {
    char *dir = 0;
    char *suit = "spade_";
    switch (cm->facing) {
        case DIRECTION_UP: { 
            suit = "cardman_";
            dir = "up_";
        } break;
        case DIRECTION_DOWN: { 
            dir = "down_";
        } break;
        case DIRECTION_LEFT: { 
            dir = "left_";
        } break;
        case DIRECTION_RIGHT: { 
            dir = "right_";
        } break;
    }
    return getPermStringForTempAnimKey(tempStringAppend(tempStringAppend(suit, dir), "katana_0"));
}

