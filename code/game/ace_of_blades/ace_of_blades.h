#ifndef ACE_OF_BLADES_H
#define ACE_OF_BLADES_H

typedef enum {
    INPUT_SOURCE_KEYBOARD,
    INPUT_SOURCE_GAMEPAD,
    INPUT_SOURCE_VIRTUAL
} input_source;

typedef struct aob_input {
    input_key up;
    input_key down;
    input_key left;
    input_key right;

    input_key attack;
    input_key dash;
    input_key playCard;
} aob_input;

typedef struct animation_state {
    char *key;
    char *prevKey;
    u32 currentFrame;
    u32 currentFrameStep;
} animation_state;

typedef char_anim_data *char_anim_data_ptr;
#define HASH_MAP_TYPE char_anim_data_ptr
#include "../hash_map.h"

typedef enum {
    CARD_SUIT_SPADE,
    CARD_SUIT_CLUB,
    CARD_SUIT_HEART,
    CARD_SUIT_DIAMOND
} card_suit;

typedef enum {
    CARD_VAL_NONE,
    CARD_VAL_ACE,
    CARD_VAL_TWO,
    CARD_VAL_THREE,
    CARD_VAL_FOUR,
    CARD_VAL_FIVE,
    CARD_VAL_SIX,
    CARD_VAL_SEVEN,
    CARD_VAL_EIGHT,
    CARD_VAL_NINE,
    CARD_VAL_TEN,
    CARD_VAL_JACK,
    CARD_VAL_QUEEN,
    CARD_VAL_KING
} card_val;

typedef enum {
    CARDMAN_STATE_IDLE,
    CARDMAN_STATE_WALKING,
} cardman_state;

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction;

typedef enum {
    CARDMAN_OWNER_PLAYER,
    CARDMAN_OWNER_CPU
} cardman_owner;

typedef struct cardman {
    b32 active;

    cardman_owner owner;
    card_suit suit;
    card_val value;

    cardman_state state;
    animation_state animState;
    direction facing;
    vec2 pos;
    vec2 vel;
} cardman;
#define MAX_NUM_CARDMEN 100

#define CARDMAN_SPEED 200.0f

typedef struct aob_state {
    cardman cardmen[MAX_NUM_CARDMEN];
    cardman *playerCardman;

    input_source inputSource;

    b32 animationsLoaded;
    char_anim_data_ptr_hash_map animations;
} aob_state;

#endif
