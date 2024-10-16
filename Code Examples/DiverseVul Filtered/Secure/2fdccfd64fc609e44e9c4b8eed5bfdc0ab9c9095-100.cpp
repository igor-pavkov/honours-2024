matroska_type_find (GstTypeFind * tf, gpointer ununsed)
{
  GstTypeFindProbability prob;
  GstMatroskaInfo info = { 0, };
  const gchar *type_name;
  DataScanCtx c = { 0, NULL, 0 };
  gboolean is_audio;
  guint64 size;
  guint32 id, hdr_len;

  if (!data_scan_ctx_ensure_data (tf, &c, 64))
    return;

  if (GST_READ_UINT32_BE (c.data) != EBML_HEADER)
    return;

  while (c.offset < EBML_MAX_LEN && data_scan_ctx_ensure_data (tf, &c, 64)) {
    hdr_len = ebml_read_chunk_header (tf, &c, c.size, &id, &size);
    if (hdr_len == 0)
      return;

    GST_INFO ("=== top-level chunk %08x, size %" G_GUINT64_FORMAT
        " / %" G_GUINT64_FORMAT, id, size, size + hdr_len);

    if (!ebml_parse_chunk (tf, &c, id, size, &info, 0))
      break;
    data_scan_ctx_advance (tf, &c, size);
    GST_INFO ("=== done with chunk %08x", id);
    if (id == MATROSKA_SEGMENT)
      break;
  }

  GST_INFO ("audio=%u video=%u other=%u chunks=%u doctype=%d all_tracks=%d",
      info.audio, info.video, info.other, info.chunks, info.doctype,
      info.tracks_ok);

  /* perhaps we should bail out if tracks_ok is FALSE and wait for more data?
   * (we would need new API to signal this properly and prevent other
   * typefinders from taking over the decision then) */
  is_audio = (info.audio > 0 && info.video == 0 && info.other == 0);

  if (info.doctype == EBML_DOCTYPE_WEBM) {
    type_name = (is_audio) ? "audio/webm" : "video/webm";
  } else if (info.video > 0 && info.video_stereo) {
    type_name = "video/x-matroska-3d";
  } else {
    type_name = (is_audio) ? "audio/x-matroska" : "video/x-matroska";
  }

  if (info.doctype == EBML_DOCTYPE_UNKNOWN)
    prob = GST_TYPE_FIND_LIKELY;
  else
    prob = GST_TYPE_FIND_MAXIMUM;

  gst_type_find_suggest_simple (tf, prob, type_name, NULL);
}