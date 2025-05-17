// sound-engine for null0 carts

#include "casio_ctx230_sf2.h"
#include "host.h"

#define TSF_IMPLEMENTATION
#include "tsf.h"

// TODO: sound/sfx/speech

static cvector_vector_type(tsf *) soundfonts;

// just get basic casio CTX-230 GM font
tsf *load_default_soundfont() {
  return tsf_load_memory(casio_ctx230_sf2, sizeof(casio_ctx230_sf2));
}

// get a soundfont (in other files)
tsf *get_soundfont(unsigned int id) {
  return soundfonts[id];
}

// load a soundfont
unsigned int add_soundfont(tsf *soundfount) {
  if (soundfount == NULL) {
    return 0;
  }
  unsigned int id = cvector_size(soundfonts);
  cvector_push_back(soundfonts, soundfount);
  return id;
}

// load a soundfont in cart
HOST_FUNCTION(unsigned int, soundfont_load, (uint32_t filenamePtr), {
  char *filename = copy_string_from_cart(filenamePtr);
  uint32_t size = 0;
  unsigned char *buffer = fs_load_file(filename, &size);
  return add_soundfont(tsf_load_memory(buffer, size));
})
