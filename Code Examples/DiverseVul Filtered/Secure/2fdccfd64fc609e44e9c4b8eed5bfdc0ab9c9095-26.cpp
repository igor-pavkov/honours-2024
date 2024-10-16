ebml_parse_chunk (GstTypeFind * tf, DataScanCtx * ctx, guint32 chunk_id,
    guint chunk_size, GstMatroskaInfo * info, guint depth)
{                               /* FIXME: make sure input size is clipped to 32 bit */
  static const gchar SPACES[] = "                ";
  DataScanCtx c = *ctx;
  guint64 element_size;
  guint32 id, hdr_len;

  if (depth >= 8)               /* keep SPACES large enough for depth */
    return FALSE;

  while (chunk_size > 0) {
    if (c.offset > EBML_MAX_LEN || !data_scan_ctx_ensure_data (tf, &c, 64))
      return FALSE;

    hdr_len = ebml_read_chunk_header (tf, &c, chunk_size, &id, &element_size);
    if (hdr_len == 0)
      return FALSE;

    g_assert (hdr_len <= chunk_size);
    chunk_size -= hdr_len;

    if (element_size > chunk_size)
      return FALSE;

    GST_DEBUG ("%s %08x, size %" G_GUINT64_FORMAT " / %" G_GUINT64_FORMAT,
        SPACES + sizeof (SPACES) - 1 - (2 * depth), id, element_size,
        hdr_len + element_size);

    if (!data_scan_ctx_ensure_data (tf, &c, element_size)) {
      GST_DEBUG ("not enough data");
      return FALSE;
    }

    switch (id) {
      case EBML_DOCTYPE:
        if (element_size >= 8 && memcmp (c.data, "matroska", 8) == 0)
          info->doctype = EBML_DOCTYPE_MATROSKA;
        else if (element_size >= 4 && memcmp (c.data, "webm", 4) == 0)
          info->doctype = EBML_DOCTYPE_WEBM;
        break;
      case MATROSKA_SEGMENT:
        GST_LOG ("parsing segment");
        ebml_parse_chunk (tf, &c, id, element_size, info, depth + 1);
        GST_LOG ("parsed segment, done");
        return FALSE;
      case MATROSKA_TRACKS:
        GST_LOG ("parsing tracks");
        info->tracks_ok =
            ebml_parse_chunk (tf, &c, id, element_size, info, depth + 1);
        GST_LOG ("parsed tracks: %s, done (after %" G_GUINT64_FORMAT " bytes)",
            info->tracks_ok ? "ok" : "FAIL", c.offset + element_size);
        return FALSE;
      case MATROSKA_TRACK_ENTRY:
        GST_LOG ("parsing track entry");
        if (!ebml_parse_chunk (tf, &c, id, element_size, info, depth + 1))
          return FALSE;
        break;
      case MATROSKA_TRACK_TYPE:{
        guint type = 0, i;

        /* is supposed to always be 1-byte, but not everyone's following that */
        for (i = 0; i < element_size; ++i)
          type = (type << 8) | c.data[i];

        GST_DEBUG ("%s   track type %u",
            SPACES + sizeof (SPACES) - 1 - (2 * depth), type);

        if (type == 1)
          ++info->video;
        else if (c.data[0] == 2)
          ++info->audio;
        else
          ++info->other;
        break;
      }
      case MATROSKA_STEREO_MODE:
        ++info->video_stereo;
        break;
      case MATROSKA_CLUSTER:
        GST_WARNING ("cluster, bailing out (should've found tracks by now)");
        return FALSE;
      default:
        break;
    }
    data_scan_ctx_advance (tf, &c, element_size);
    chunk_size -= element_size;
    ++info->chunks;
  }

  return TRUE;
}