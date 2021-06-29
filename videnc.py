import sys
from rle import rle_enc, bits

BYTES_NUM = (int(sys.argv[1]) * int(sys.argv[2])) // 8


def fullenc(b):
  return b'\x01' + b


def fullenc_rle(b):
  return b'\x02' + b''.join(rle_enc(b))


def unchanged():
  return b'\x00'


# TODO find a better way to encode these
def delta_rle(byts, prev):
  if prev is None:
    return None

  # The idea: get the XOR of the bytes, then RLE compress it
  xored = bytes(map(lambda a,b: a ^ b, byts, prev))

  assert len(xored) == len(byts)
  assert len(byts) == len(prev)

  return b'\x03' + b''.join(rle_enc(xored))


def blank(b):
  if b == b"\x00" * BYTES_NUM:
    return b'\x04'
  elif b == b"\xFF" * BYTES_NUM:
    return b'\x05'
  else:
    return None


# Some more codes could be made:
# Blank variants that SET a flat color rather than retain previous frames
def encoder():
  prev_frame = None

  with open("bad_mono", "rb") as f:
    while (byts := f.read(BYTES_NUM)):
      if byts == prev_frame:
        yield unchanged()
      else:
        blk = blank(byts)
        if blk is not None:
          yield blk
        else:
          full = fullenc(byts)
          rle = fullenc_rle(byts)
          dlt = delta_rle(byts, prev_frame)
          if dlt is not None and len(dlt) < len(rle) and len(dlt) < len(full):
            yield dlt
          elif len(rle) < len(full):
            yield rle
          else:
            yield full

        prev_frame = byts


if __name__ == "__main__":
  with open("bad_mono.enc", "wb") as f:
    for chunk in encoder():
      f.write(chunk)

  #import heatshrink2

  #with heatshrink2.open("bad_mono.enc.bin", "wb") as w:
  #  with open("bad_mono.enc", "rb") as f:
  #    w.write(f.read())
