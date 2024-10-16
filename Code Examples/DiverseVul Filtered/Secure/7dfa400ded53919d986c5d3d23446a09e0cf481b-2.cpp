int processing_start(png_structp& png_ptr, png_infop& info_ptr, void* frame_ptr,
                     bool hasInfo, CHUNK& chunkIHDR,
                     std::vector<CHUNK>& chunksInfo) {
  unsigned char header[8] = {137, 80, 78, 71, 13, 10, 26, 10};

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info_ptr = png_create_info_struct(png_ptr);
  if (!png_ptr || !info_ptr) return 1;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return 1;
  }

  png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
  png_set_progressive_read_fn(png_ptr, frame_ptr, info_fn, row_fn, NULL);

  png_process_data(png_ptr, info_ptr, header, 8);
  png_process_data(png_ptr, info_ptr, chunkIHDR.p, chunkIHDR.size);

  if (hasInfo) {
    for (unsigned int i = 0; i < chunksInfo.size(); i++) {
      png_process_data(png_ptr, info_ptr, chunksInfo[i].p, chunksInfo[i].size);
    }
  }
  return 0;
}