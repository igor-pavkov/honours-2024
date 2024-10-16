static int mov_write_enda_tag_be(AVIOContext *pb)
{
  avio_wb32(pb, 10);
  ffio_wfourcc(pb, "enda");
  avio_wb16(pb, 0); /* big endian */
  return 10;
}