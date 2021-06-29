def bits(n):
  for x in range(8):
    yield (n >> (7-x)) & 1


def rle_enc_1(byts):
  count = 0
  prev = None

  for byt in byts:
    for b in bits(byt):
      if prev is None:
        prev = b
      if prev == b:
        count += 1
      else:
        yield (count, prev)
        prev = b
        count = 1
  yield (count, prev)


def rle_enc(byts):
  for count, state in rle_enc_1(byts):
    assert count != 0
    number_of_full = count // 0b1111111
    remainder = count % 0b1111111

    if remainder != 0:
      yield ((0b11111110 | state).to_bytes(1, byteorder="little") * number_of_full) + ((remainder << 1) | state).to_bytes(1, byteorder="little")
    else:
      yield ((0b11111110 | state).to_bytes(1, byteorder="little") * number_of_full)


if __name__ == "__main__":
  with open("bad_mono", "rb") as f:
    byts = f.read()

  with open("bad_mono.rle", "wb") as w:
    for bts in rle_enc(byts): w.write(bts)

  #import heatshrink2

  #with heatshrink2.open('bad_mono.rle.bin', 'wb') as w:
  #  with open("bad_mono.rle", "rb") as f:
  #    w.write(f.read())

  #with heatshrink2.open('bad_mono.bin', 'wb') as w:
  #  with open("bad_mono", "rb") as f:
  #    w.write(f.read())

