// Extra for experts: add heatshrink support
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define W (80)
#define H (64)
#define PIXELS (W * H)
#define VRAM_BYTES (PIXELS / 8)
#define MIN_RLE_SIZE (PIXELS / 128)

static uint8_t vram[VRAM_BYTES] = {0};
static uint8_t buf[MIN_RLE_SIZE] = {0};
static uint16_t pixels = 0;
static FILE *f = NULL;

static void full() { assert(fread(vram, VRAM_BYTES, 1, f) == 1); }

#include "rle_dec.h"
#include "rle_dec_d.h"

static void full_rle() {
  pixels = 0;
  // By reading a lot at once this will
  // mean less read calls
  assert(fread(buf, MIN_RLE_SIZE, 1, f) == 1);
  for (uint8_t i = 0; i < MIN_RLE_SIZE; i++) {
    rle_dec(buf[i]);
  }
  while (pixels < PIXELS) {
    assert(fread(buf, 1, 1, f) == 1);
    rle_dec(buf[0]);
  }
}

static void delta() {
  pixels = 0;
  // By reading a lot at once this will
  // mean less read calls
  assert(fread(buf, MIN_RLE_SIZE, 1, f) == 1);
  for (uint8_t i = 0; i < MIN_RLE_SIZE; i++) {
    rle_dec_d(buf[i]);
  }
  while (pixels < PIXELS) {
    assert(fread(buf, 1, 1, f) == 1);
    rle_dec_d(buf[0]);
  }
}

int main() {
  f = fopen("bad_mono.enc", "rb");
  if (!f)
    return 1;

  while (fread(buf, 1, 1, f) == 1) {
    // fprintf(stderr, "> %d\n", buf[0]);
    switch (buf[0]) {
    case 0: // Blank
      break;
    case 1: // Full
      full();
      break;
    case 2: // Full (RLE)
      full_rle();
      break;
    case 3: // Delta
      delta();
      break;
    default:
      assert(69 == 420);
      break;
    }
    fwrite(vram, VRAM_BYTES, 1, stdout);
  }

  fclose(f);
  return 0;
}
