// Extra for experts: add heatshrink support
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define W (80)
#define H (64)
#define PIXELS (W * H)
#define VRAM_BYTES (PIXELS / 8)
#define MIN_RLE_SIZE (PIXELS / 128)

static uint8_t vram[VRAM_BYTES] = {0};
static uint8_t buf[MIN_RLE_SIZE] = {0};
static uint16_t pixels = 0;
static FILE *f = NULL;

static void full() {
    assert(fread(vram, VRAM_BYTES, 1, f) == 1);
}

static void rle_dec(uint8_t data) {
    int8_t n = data >> 1;
    uint8_t on = data & 1;
    //fprintf(stderr, "%d %d %d %d %d\n", i, buf[i], pixels, n, on);
    assert(n != 0);
    uint8_t offset = pixels % 8;
    uint8_t old_vram = vram[pixels / 8];
    if (offset != 0) {
        // Set from point unwritten to, to end of byte
        if (on != 0) {
            vram[pixels / 8] |= 0xFF >> offset;
        } else {
            vram[pixels / 8] &= ~(0xFF >> offset);
        }
        // If the rest of the byte is entirely filled
        if (n >= 8 - offset) {
            pixels += 8 - offset;
            n -= 8 - offset;
            assert(pixels % 8 == 0);
        } else {
            // Overwrote too much, need to correct
            pixels += 8 - offset;
            n -= 8 - offset;
            assert(n < 0);
            vram[pixels / 8] &= 0xFF << -n;
            vram[pixels / 8] |= old_vram & ~(0xFF << -n);
            pixels += n;
            n = 0;
        }
    }
    // Any overlap will be corrected below
    while (n > 0) {
        old_vram = vram[pixels / 8];
        vram[pixels / 8] = 0xFF * on;
        pixels += 8;
        n -= 8;
    }
    // overlap
    if (n != 0) {
        assert(n < 0);
        // Overwrote too much, need to correct
        // THis is buggy
        vram[pixels / 8] &= 0xFF << -n;
        vram[pixels / 8] |= old_vram & ~(0xFF << -n);
        pixels += n;
        n = 0;
    }
}

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

// TODO
static void delta() { assert(1 == 0); /*
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
*/ }

int main() {
    f = fopen("bad_mono.enc", "rb");
    if (!f) return 1;

    while (fread(buf, 1, 1, f) == 1) {
        //fprintf(stderr, "> %d\n", buf[0]);
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
