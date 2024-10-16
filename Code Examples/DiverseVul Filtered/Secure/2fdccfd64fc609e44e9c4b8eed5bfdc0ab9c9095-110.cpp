aac_type_find_scan_loas_frames_ep (GstTypeFind * tf, DataScanCtx * scan_ctx,
    gint max_frames)
{
  DataScanCtx c = *scan_ctx;
  guint16 snc;
  guint len;
  gint count = 0;

  do {
    if (!data_scan_ctx_ensure_data (tf, &c, 5))
      break;

    /* EPAudioSyncStream */
    len = ((c.data[2] & 0x0f) << 9) | (c.data[3] << 1) |
        ((c.data[4] & 0x80) >> 7);

    if (len == 0 || !data_scan_ctx_ensure_data (tf, &c, len + 2)) {
      GST_DEBUG ("Wrong sync or next frame not within reach, len=%u", len);
      break;
    }

    /* check length of frame  */
    snc = GST_READ_UINT16_BE (c.data + len);
    if (snc != 0x4de1) {
      GST_DEBUG ("No sync found at 0x%" G_GINT64_MODIFIER "x", c.offset + len);
      break;
    }

    ++count;

    GST_DEBUG ("Found LOAS syncword #%d at offset 0x%" G_GINT64_MODIFIER "x, "
        "framelen %u", count, c.offset, len);

    data_scan_ctx_advance (tf, &c, len);
  } while (count < max_frames && (c.offset - scan_ctx->offset) < 64 * 1024);

  GST_DEBUG ("found %d consecutive frames", count);
  return count;
}