inline unsigned int read_chunk(Reader* r, CHUNK* pChunk) {
  unsigned char len[4];
  pChunk->size = 0;
  pChunk->p = 0;
  if (r->Read(&len, 4)) {
    const auto size = png_get_uint_32(len);
    // Check first, to avoid overflow.
    if (size > kMaxPNGChunkSize) {
      JXL_WARNING("APNG chunk size is too big");
      return 0;
    }
    pChunk->size = size + 12;
    pChunk->p = new unsigned char[pChunk->size];
    memcpy(pChunk->p, len, 4);
    if (r->Read(pChunk->p + 4, pChunk->size - 4)) {
      return *(unsigned int*)(pChunk->p + 4);
    }
  }
  return 0;
}