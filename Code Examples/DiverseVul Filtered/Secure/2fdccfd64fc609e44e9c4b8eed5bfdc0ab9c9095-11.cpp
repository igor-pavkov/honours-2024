aac_type_find_scan_loas_frames (GstTypeFind * tf, DataScanCtx * scan_ctx,
    gint max_frames)
{
  DataScanCtx c = *scan_ctx;
  guint16 snc;
  guint len;
  gint count = 0;

  do {
    if (!data_scan_ctx_ensure_data (tf, &c, 3))
      break;

    /* AudioSyncStream */
    len = ((c.data[1] & 0x1f) << 8) | c.data[2];
    /* add size of sync stream header */
    len += 3;

    if (len == 0 || !data_scan_ctx_ensure_data (tf, &c, len + 2)) {
      GST_DEBUG ("Wrong sync or next frame not within reach, len=%u", len);
      break;
    }

    /* check length of frame  */
    snc = GST_READ_UINT16_BE (c.data + len);
    if ((snc & 0xffe0) != 0x56e0) {
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