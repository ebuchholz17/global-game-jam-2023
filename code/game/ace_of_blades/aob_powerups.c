powerup_type getUpgradeTypeForSuitValue ( card_suit suit, card_val value) {
    switch (suit) {
        case CARD_SUIT_SPADE: {
            switch (value) {
                case CARD_VAL_ACE: {
                   return POWERUP_TYPE_ATTACK_COMBO;
                } break;
                case CARD_VAL_TWO: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_THREE: {
                   return POWERUP_TYPE_DASH;
                } break;
                case CARD_VAL_FOUR: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_FIVE: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
                case CARD_VAL_SIX: {
                   return POWERUP_TYPE_INCREASE_ENEMY_HITSTUN;
                } break;
                case CARD_VAL_SEVEN: {
                   return POWERUP_TYPE_DASH;
                } break;
                case CARD_VAL_EIGHT: {
                   return POWERUP_TYPE_DASH;
                } break;
                case CARD_VAL_NINE: {
                   return POWERUP_TYPE_DECREASE_PLAYER_HITSTUN;
                } break;
                case CARD_VAL_TEN: {
                   return POWERUP_TYPE_DECREASE_PLAYER_HITSTUN;
                } break;
                case CARD_VAL_JACK: {
                   return POWERUP_TYPE_KNOCKBACK;
                } break;
                case CARD_VAL_QUEEN: {
                   return POWERUP_TYPE_INCREASE_ENEMY_HITSTUN;
                } break;
                case CARD_VAL_KING: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
            }
        } break;
        case CARD_SUIT_CLUB: {
            switch (value) {
                case CARD_VAL_ACE: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
                case CARD_VAL_TWO: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
                case CARD_VAL_THREE: {
                   return POWERUP_TYPE_BACKSTEP;
                } break;
                case CARD_VAL_FOUR: {
                   return POWERUP_TYPE_KNOCKBACK;
                } break;
                case CARD_VAL_FIVE: {
                   return POWERUP_TYPE_KNOCKBACK;
                } break;
                case CARD_VAL_SIX: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_SEVEN: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_EIGHT: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
                case CARD_VAL_NINE: {
                   return POWERUP_TYPE_ATTACK_SPEED;
                } break;
                case CARD_VAL_TEN: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_JACK: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_QUEEN: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_KING: {
                   return POWERUP_TYPE_INVINCIBILITY_FRAMES;
                } break;
            }
        } break;
        case CARD_SUIT_DIAMOND: {
            switch (value) {
                case CARD_VAL_ACE: {
                   return POWERUP_TYPE_INCREASE_ENEMY_HITSTUN;
                } break;
                case CARD_VAL_TWO: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_THREE: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_FOUR: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_FIVE: {
                   return POWERUP_TYPE_DASH;
                } break;
                case CARD_VAL_SIX: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_SEVEN: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_EIGHT: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_NINE: {
                   return POWERUP_TYPE_KNOCKBACK;
                } break;
                case CARD_VAL_TEN: {
                   return POWERUP_TYPE_INVINCIBILITY_FRAMES;
                } break;
                case CARD_VAL_JACK: {
                   return POWERUP_TYPE_SPEED;
                } break;
                case CARD_VAL_QUEEN: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_KING: {
                   return POWERUP_TYPE_DASH;
                } break;
            }
        } break;
        case CARD_SUIT_HEART: {
            switch (value) {
                case CARD_VAL_ACE: {
                   return POWERUP_TYPE_ATTACK_COMBO;
                } break;
                case CARD_VAL_TWO: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_THREE: {
                   return POWERUP_TYPE_INVINCIBILITY_FRAMES;
                } break;
                case CARD_VAL_FOUR: {
                   return POWERUP_TYPE_DECREASE_PLAYER_HITSTUN;
                } break;
                case CARD_VAL_FIVE: {
                   return POWERUP_TYPE_BACKSTEP;
                } break;
                case CARD_VAL_SIX: {
                   return POWERUP_TYPE_BACKSTEP;
                } break;
                case CARD_VAL_SEVEN: {
                   return POWERUP_TYPE_HITPOINTS;
                } break;
                case CARD_VAL_EIGHT: {
                   return POWERUP_TYPE_INCREASE_ENEMY_HITSTUN;
                } break;
                case CARD_VAL_NINE: {
                   return POWERUP_TYPE_BACKSTEP;
                } break;
                case CARD_VAL_TEN: {
                   return POWERUP_TYPE_ATTACK_DAMAGE;
                } break;
                case CARD_VAL_JACK: {
                   return POWERUP_TYPE_INVINCIBILITY_FRAMES;
                } break;
                case CARD_VAL_QUEEN: {
                   return POWERUP_TYPE_BACKSTEP;
                } break;
                case CARD_VAL_KING: {
                   return POWERUP_TYPE_DECREASE_PLAYER_HITSTUN;
                } break;
            }
        } break;
    }
    //default, shouldn't reach here
    return POWERUP_TYPE_SPEED;
}

f32 getMoveSpeedForLevel (u32 level) {
    return 130.0f + 15.0f * (f32)level;
}

f32 getEnemySpeedForLevel (u32 level) {
    return 50.0f + 15.0f * (f32)level;
}

f32 getHitpointsForLevel (u32 level) {
    return 100.0f + 10.0f * (f32)level;
}

f32 getCPUHitpointsForLevel (u32 level) {
    return 30.0f + 20.0f * (f32)level;
}

f32 getDashSpeedForLevel (u32 level) {
    return 150.0f + 30.0f * (f32)level;
}

f32 getBackstepSpeedForLevel (u32 level) {
    return 100.0f + 30.0f * (f32)level;
}

f32 getAttackDamageMultiplierForLevel (u32 level) {
    return 1.0f + 0.2f * (f32)level;
}

f32 getAttackSpeedMultiplierForLevel (u32 level) {
    return 1.0f + 0.1f * (f32)level;
}

f32 getKnockbackForLevel (u32 level) {
    return 450.0f + 150.0f * (f32)level;
}

f32 getEnemyHitstunForLevel (u32 level) {
    return 0.5f + 0.25f * (f32)level;
}

f32 getPlayerHitstunForLevel (u32 level) {
    f32 hitstun = 0.5f - 0.1f * (f32)level;
    if (hitstun < 0.0f) { hitstun = 0.0f; }
    return hitstun;
}

f32 getInvincibilityFramesForLevel (u32 level) {
    return 0.1f + 0.15f * (f32)level;
}

