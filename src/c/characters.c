#include "characters.h"

static const uint32_t s_res[CHARACTER_COUNT][MOOD_COUNT] = {
  { // Mia
    RESOURCE_ID_IMAGE_FACE_MIA_RELAXED,
    RESOURCE_ID_IMAGE_FACE_MIA_CALM,
    RESOURCE_ID_IMAGE_FACE_MIA_ALERT,
    RESOURCE_ID_IMAGE_FACE_MIA_ANXIOUS,
  },
  { // Kai
    RESOURCE_ID_IMAGE_FACE_KAI_RELAXED,
    RESOURCE_ID_IMAGE_FACE_KAI_CALM,
    RESOURCE_ID_IMAGE_FACE_KAI_ALERT,
    RESOURCE_ID_IMAGE_FACE_KAI_ANXIOUS,
  },
  { // Nova
    RESOURCE_ID_IMAGE_FACE_NOVA_RELAXED,
    RESOURCE_ID_IMAGE_FACE_NOVA_CALM,
    RESOURCE_ID_IMAGE_FACE_NOVA_ALERT,
    RESOURCE_ID_IMAGE_FACE_NOVA_ANXIOUS,
  },
  { // Rex
    RESOURCE_ID_IMAGE_FACE_REX_RELAXED,
    RESOURCE_ID_IMAGE_FACE_REX_CALM,
    RESOURCE_ID_IMAGE_FACE_REX_ALERT,
    RESOURCE_ID_IMAGE_FACE_REX_ANXIOUS,
  },
  { // Cat
    RESOURCE_ID_IMAGE_FACE_CAT_RELAXED,
    RESOURCE_ID_IMAGE_FACE_CAT_CALM,
    RESOURCE_ID_IMAGE_FACE_CAT_ALERT,
    RESOURCE_ID_IMAGE_FACE_CAT_ANXIOUS,
  },
  { // Dog
    RESOURCE_ID_IMAGE_FACE_DOG_RELAXED,
    RESOURCE_ID_IMAGE_FACE_DOG_CALM,
    RESOURCE_ID_IMAGE_FACE_DOG_ALERT,
    RESOURCE_ID_IMAGE_FACE_DOG_ANXIOUS,
  },
  { // Fox
    RESOURCE_ID_IMAGE_FACE_FOX_RELAXED,
    RESOURCE_ID_IMAGE_FACE_FOX_CALM,
    RESOURCE_ID_IMAGE_FACE_FOX_ALERT,
    RESOURCE_ID_IMAGE_FACE_FOX_ANXIOUS,
  },
  { // Owl
    RESOURCE_ID_IMAGE_FACE_OWL_RELAXED,
    RESOURCE_ID_IMAGE_FACE_OWL_CALM,
    RESOURCE_ID_IMAGE_FACE_OWL_ALERT,
    RESOURCE_ID_IMAGE_FACE_OWL_ANXIOUS,
  },
};

const char *character_name(CharacterId id) {
  switch (id) {
    case CHAR_MIA:  return "Mia";
    case CHAR_KAI:  return "Kai";
    case CHAR_NOVA: return "Nova";
    case CHAR_REX:  return "Rex";
    case CHAR_CAT:  return "Cat";
    case CHAR_DOG:  return "Dog";
    case CHAR_FOX:  return "Fox";
    case CHAR_OWL:  return "Owl";
    default:        return "Mia";
  }
}

uint32_t character_resource(CharacterId id, MoodLevel mood) {
  unsigned cid = (unsigned)id;
  unsigned mid = (unsigned)mood;
  if (cid >= CHARACTER_COUNT) {
    cid = CHAR_MIA;
  }
  if (mid >= MOOD_COUNT) {
    mid = MOOD_CALM;
  }
  return s_res[cid][mid];
}
