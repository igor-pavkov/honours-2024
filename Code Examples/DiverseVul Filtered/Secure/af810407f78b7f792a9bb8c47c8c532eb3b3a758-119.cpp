static void flush_checksum(ha_checksum *row_crc, uchar **checksum_start,
                           size_t *checksum_length)
{
  if (*checksum_start)
  {
    *row_crc= my_checksum(*row_crc, *checksum_start, *checksum_length);
    *checksum_start= NULL;
    *checksum_length= 0;
  }
}