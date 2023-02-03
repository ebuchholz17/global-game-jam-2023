#include "aob_cardman_anims.h"

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
        return tempStringAppend(result, "idle");
    }
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
        return tempStringAppend(result, "run");
    }
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

void startAnimState (animation_state *animState) {
    animState->currentFrameStep = 0;
    animState->currentFrame = 0;
}

b32 updateAnimState (animation_state *animState) {
    char_anim_data *animData = char_anim_data_ptr_hash_mapGetVal(&aob->animations, animState->key);
    char_frame_data *currentFrame = &animData->frames[animState->currentFrame];

    b32 animationComplete = false;
    ++animState->currentFrameStep;
    if (animState->currentFrameStep >= currentFrame->duration) {
        animState->currentFrameStep = 0;
        ++animState->currentFrame;
        if (animState->currentFrame == animData->numFrames) {
            animState->currentFrame = 0;
            animationComplete = true;
        }
    }
    return animationComplete;
}

