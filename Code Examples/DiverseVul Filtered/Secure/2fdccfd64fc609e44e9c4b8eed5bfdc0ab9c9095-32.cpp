ac3_type_find (GstTypeFind * tf, gpointer unused)
{
  DataScanCtx c = { 0, NULL, 0 };

  /* Search for an ac3 frame; not necessarily right at the start, but give it
   * a lower probability if not found right at the start. Check that the
   * frame is followed by a second frame at the expected offset.
   * We could also check the two ac3 CRCs, but we don't do that right now */
  while (c.offset < 1024) {
    if (G_UNLIKELY (!data_scan_ctx_ensure_data (tf, &c, 5)))
      break;

    if (c.data[0] == 0x0b && c.data[1] == 0x77) {
      guint bsid = c.data[5] >> 3;

      if (bsid <= 8) {
        /* ac3 */
        guint fscod = c.data[4] >> 6;
        guint frmsizecod = c.data[4] & 0x3f;

        if (fscod < 3 && frmsizecod < 38) {
          DataScanCtx c_next = c;
          guint frame_size;

          frame_size = ac3_frmsizecod_tbl[frmsizecod].frm_size[fscod];
          GST_LOG ("possible AC3 frame sync at offset %"
              G_GUINT64_FORMAT ", size=%u", c.offset, frame_size);
          if (data_scan_ctx_ensure_data (tf, &c_next, (frame_size * 2) + 5)) {
            data_scan_ctx_advance (tf, &c_next, frame_size * 2);

            if (c_next.data[0] == 0x0b && c_next.data[1] == 0x77) {
              fscod = c_next.data[4] >> 6;
              frmsizecod = c_next.data[4] & 0x3f;

              if (fscod < 3 && frmsizecod < 38) {
                GstTypeFindProbability prob;

                GST_LOG ("found second AC3 frame (size=%u), looks good",
                    ac3_frmsizecod_tbl[frmsizecod].frm_size[fscod]);
                if (c.offset == 0)
                  prob = GST_TYPE_FIND_MAXIMUM;
                else
                  prob = GST_TYPE_FIND_NEARLY_CERTAIN;

                gst_type_find_suggest (tf, prob, AC3_CAPS);
                return;
              }
            } else {
              GST_LOG ("no second AC3 frame found, false sync");
            }
          }
        }
      } else if (bsid <= 16 && bsid > 10) {
        /* eac3 */
        DataScanCtx c_next = c;
        guint frame_size;

        frame_size = (((c.data[2] & 0x07) << 8) + c.data[3]) + 1;
        GST_LOG ("possible E-AC3 frame sync at offset %"
            G_GUINT64_FORMAT ", size=%u", c.offset, frame_size);
        if (data_scan_ctx_ensure_data (tf, &c_next, (frame_size * 2) + 5)) {
          data_scan_ctx_advance (tf, &c_next, frame_size * 2);

          if (c_next.data[0] == 0x0b && c_next.data[1] == 0x77) {
            GstTypeFindProbability prob;

            GST_LOG ("found second E-AC3 frame, looks good");
            if (c.offset == 0)
              prob = GST_TYPE_FIND_MAXIMUM;
            else
              prob = GST_TYPE_FIND_NEARLY_CERTAIN;

            gst_type_find_suggest (tf, prob, EAC3_CAPS);
            return;
          } else {
            GST_LOG ("no second E-AC3 frame found, false sync");
          }
        }
      } else {
        GST_LOG ("invalid AC3 BSID: %u", bsid);
      }
    }
    data_scan_ctx_advance (tf, &c, 1);
  }
}