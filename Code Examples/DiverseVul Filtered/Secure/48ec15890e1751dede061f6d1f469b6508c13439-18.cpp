read_block_header (FILE    *f,
                   guint32 *init_len,
                   guint32 *total_len)
{
  guchar buf[4];
  guint16 id;
  long header_start;
  guint32 len;

  IFDBG(3) header_start = ftell (f);

  if (fread (buf, 4, 1, f) < 1
      || fread (&id, 2, 1, f) < 1
      || fread (&len, 4, 1, f) < 1
      || (psp_ver_major < 4 && fread (total_len, 4, 1, f) < 1))
    {
      g_message ("Error reading block header");
      return -1;
    }
  if (memcmp (buf, "~BK\0", 4) != 0)
    {
      IFDBG(3)
        g_message ("Invalid block header at %ld", header_start);
      else
        g_message ("Invalid block header");
      return -1;
    }

  IFDBG(3) g_message ("%s at %ld", block_name (id), header_start);

  if (psp_ver_major < 4)
    {
      *init_len = GUINT32_FROM_LE (len);
      *total_len = GUINT32_FROM_LE (*total_len);
    }
  else
    {
      /* Version 4.0 seems to have dropped the initial data chunk length
       * field.
       */
      *init_len = 0xDEADBEEF;   /* Intentionally bogus, should not be used */
      *total_len = GUINT32_FROM_LE (len);
    }

  return GUINT16_FROM_LE (id);
}