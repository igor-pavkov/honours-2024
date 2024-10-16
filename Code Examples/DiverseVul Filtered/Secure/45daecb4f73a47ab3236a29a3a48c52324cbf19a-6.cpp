static unsigned int read_available_partition_size(
    VP8D_COMP *pbi, const unsigned char *token_part_sizes,
    const unsigned char *fragment_start,
    const unsigned char *first_fragment_end, const unsigned char *fragment_end,
    int i, int num_part) {
  VP8_COMMON *pc = &pbi->common;
  const unsigned char *partition_size_ptr = token_part_sizes + i * 3;
  unsigned int partition_size = 0;
  ptrdiff_t bytes_left = fragment_end - fragment_start;
  /* Calculate the length of this partition. The last partition
   * size is implicit. If the partition size can't be read, then
   * either use the remaining data in the buffer (for EC mode)
   * or throw an error.
   */
  if (i < num_part - 1) {
    if (read_is_valid(partition_size_ptr, 3, first_fragment_end)) {
      partition_size = read_partition_size(pbi, partition_size_ptr);
    } else if (pbi->ec_active) {
      partition_size = (unsigned int)bytes_left;
    } else {
      vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
                         "Truncated partition size data");
    }
  } else {
    partition_size = (unsigned int)bytes_left;
  }

  /* Validate the calculated partition length. If the buffer
   * described by the partition can't be fully read, then restrict
   * it to the portion that can be (for EC mode) or throw an error.
   */
  if (!read_is_valid(fragment_start, partition_size, fragment_end)) {
    if (pbi->ec_active) {
      partition_size = (unsigned int)bytes_left;
    } else {
      vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
                         "Truncated packet or corrupt partition "
                         "%d length",
                         i + 1);
    }
  }
  return partition_size;
}