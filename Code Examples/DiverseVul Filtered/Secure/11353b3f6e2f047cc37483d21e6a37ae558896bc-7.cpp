gst_h264_parser_identify_nalu_avc (GstH264NalParser * nalparser,
    const guint8 * data, guint offset, gsize size, guint8 nal_length_size,
    GstH264NalUnit * nalu)
{
  GstBitReader br;

  memset (nalu, 0, sizeof (*nalu));

  if (size < offset + nal_length_size) {
    GST_DEBUG ("Can't parse, buffer has too small size %" G_GSIZE_FORMAT
        ", offset %u", size, offset);
    return GST_H264_PARSER_ERROR;
  }

  size = size - offset;
  gst_bit_reader_init (&br, data + offset, size);

  nalu->size = gst_bit_reader_get_bits_uint32_unchecked (&br,
      nal_length_size * 8);
  nalu->sc_offset = offset;
  nalu->offset = offset + nal_length_size;

  if (size < nalu->size + nal_length_size) {
    nalu->size = 0;

    return GST_H264_PARSER_NO_NAL_END;
  }

  nalu->data = (guint8 *) data;

  if (!gst_h264_parse_nalu_header (nalu)) {
    GST_WARNING ("error parsing \"NAL unit header\"");
    nalu->size = 0;
    return GST_H264_PARSER_BROKEN_DATA;
  }

  nalu->valid = TRUE;

  return GST_H264_PARSER_OK;
}