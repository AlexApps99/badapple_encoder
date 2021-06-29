static void rle_dec(uint8_t data) {
  int8_t n = data >> 1;
  uint8_t on = data & 1;
  // fprintf(stderr, "%d %d %d %d %d\n", i, buf[i], pixels, n, on);
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
      n -= 8 - offset;
      assert(n < 0);
      vram[pixels / 8] &= 0xFF << -n;
      vram[pixels / 8] |= old_vram & ~(0xFF << -n);
      pixels += 8 - offset + n;
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
    pixels -= 8;
    vram[pixels / 8] &= 0xFF << -n;
    vram[pixels / 8] |= old_vram & ~(0xFF << -n);
    pixels += 8;
    pixels += n;
    n = 0;
  }
}
