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
    f32 t;
    f32 speedMultiplier;
    u32 currentFrame;
    u32 currentFrameStep;
    u32 totalFrames;
} animation_state;

typedef char_anim_data *char_anim_data_ptr;
#define HASH_MAP_TYPE char_anim_data_ptr
#include "../hash_map.h"

typedef enum {
    CARD_SUIT_SPADE,
    CARD_SUIT_CLUB,
    CARD_SUIT_HEART,
    CARD_SUIT_DIAMOND,
    CARD_SUIT_COUNT
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
    CARD_VAL_KING,
    CARD_VAL_COUNT
} card_val;

typedef enum {
    CARDMAN_STATE_IDLE,
    CARDMAN_STATE_WALKING,
    CARDMAN_STATE_ATTACKING,
    CARDMAN_STATE_HITSTUN,
    CARDMAN_STATE_DEFEATED,
    CARDMAN_STATE_FADING_OUT,
    CARDMAN_STATE_PLAYING_CARD
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

typedef struct cardman_attack {
    b32 active;

    char *animKey;

    u32 comboIndex;
    f32 damage;
    f32 damageMultiplier;
    u32 linkStartFrame;

    f32 baseSpeed;
    f32 speedMultiplier;
} cardman_attack;

typedef struct cardman_hitby_info {
    b32 wasHit;
    vec2 attackOrigin;
    f32 damage;
    f32 knockbackSpeed;
} cardman_hitby_info;

typedef struct cardman {
    b32 active;

    cardman_owner owner;
    card_suit suit;
    card_val value;

    cardman_attack attack;

    // TODO: stats

    cardman_state state;
    animation_state animState;
    direction facing;
    vec2 pos;
    vec2 vel;
    vec2 knockbackVel;

    // reset every frame
    cardman_hitby_info hitByInfo;

    f32 knockbackTimer;
    f32 hitstunTimer;
    f32 iframesTimer;

    f32 defeatedTimer;
    f32 fadingTimer;

    f32 hitPoints;
} cardman;
typedef cardman *cardman_ptr;
#define LIST_TYPE cardman_ptr
#include "list.h"

typedef struct cardman_collision {
    cardman *first;
    cardman *second;
} cardman_collision;
#define LIST_TYPE cardman_collision
#include "list.h"

#define MAX_NUM_CARDMEN 100

#define CARDMAN_SPEED 150.0f
#define DIST_TO_PLAY_CARD 300.0f
#define DIST_TO_PLAY_CARD_TO_DECK 50.0f

typedef struct scratch_mem_save{
    void *current;
} scratch_mem_save;

typedef enum {
    PLAYED_CARD_PHASE_MOVE_CARD_ABOVE,
    PLAYED_CARD_PHASE_CARD_PAUSE,
    PLAYED_CARD_PHASE_MOVE_CARD_TO_PLAYER,
    PLAYED_CARD_PHASE_MOVE_PLAYER_PAUSE
} played_card_phase;

typedef struct played_card_info {
    b32 active;

    card_suit suit;
    card_val value;
    vec2 cardStartPos;
    vec2 cardPos;

    played_card_phase phase;
    f32 t;
} played_card_info;

typedef enum {
    PLAYED_CARD_TO_DECK_PHASE_MOVE_CARD_TO_DECK,
    PLAYED_CARD_TO_DECK_PHASE_CARD_PAUSE,
    PLAYED_CARD_TO_DECK_PHASE_FLIP_CARD
} played_card_to_deck_phase;

typedef struct played_card_to_deck_info {
    b32 active;

    card_suit suit;
    card_val value;
    vec2 cardStartPos;
    vec2 cardPos;

    played_card_to_deck_phase phase;
    f32 t;
} played_card_to_deck_info;

typedef struct deck_card {
    card_suit suit;
    card_val value;
} deck_card;
#define LIST_TYPE deck_card
#include "list.h"

typedef struct deck {
    deck_card_list cards;
    vec2 pos;
    played_card_to_deck_info playedCardToDeckInfo;
} deck;

typedef struct player_upgrade {
    card_suit suit;
    card_val value;
} player_upgrade;
#define LIST_TYPE player_upgrade
#include "list.h"

#define NUM_DECKS 4
#define STARTING_CARDS_PER_DECK 5

typedef struct aob_state {
    b32 initialized;

    cardman cardmen[MAX_NUM_CARDMEN];
    cardman *playerCardman;

    played_card_info playedCardInfo;

    input_source inputSource;

    scratch_mem_save scratchMemSave;

    deck decks[NUM_DECKS];
    player_upgrade_list upgrades;

    b32 animationsLoaded;
    char_anim_data_ptr_hash_map animations;
} aob_state;

#endif
