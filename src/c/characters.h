#pragma once

#include <pebble.h>
#include "mood.h"

#define CHARACTER_COUNT 8

typedef enum {
  CHAR_MIA = 0,
  CHAR_KAI,
  CHAR_NOVA,
  CHAR_REX,
  CHAR_CAT,
  CHAR_DOG,
  CHAR_FOX,
  CHAR_OWL
} CharacterId;

const char *character_name(CharacterId id);
uint32_t character_resource(CharacterId id, MoodLevel mood);
