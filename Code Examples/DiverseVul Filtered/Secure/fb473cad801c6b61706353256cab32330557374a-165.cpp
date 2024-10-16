  (flatpak_dir_log) (FlatpakDir * self,
                     const char *file,
                     int line,
                     const char *func,
                     const char *source, /* overrides self->name */
                     const char *change,
                     const char *remote,
                     const char *ref,
                     const char *commit,
                     const char *old_commit,
                     const char *url,
                     const char *format,
                     ...)
{
#ifdef HAVE_LIBSYSTEMD
  const char *installation = source ? source : flatpak_dir_get_name_cached (self);
  pid_t source_pid = flatpak_dir_get_source_pid (self);
  char message[1024];
  int len;
  va_list args;

  len = g_snprintf (message, sizeof (message), "%s: ", installation);

  va_start (args, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  g_vsnprintf (message + len, sizeof (message) - len, format, args);
#pragma GCC diagnostic pop

  va_end (args);

  /* See systemd.journal-fields(7) for the meaning of the
   * standard fields we use, in particular OBJECT_PID
   */
  sd_journal_send ("MESSAGE_ID=" FLATPAK_MESSAGE_ID,
                   "PRIORITY=5",
                   "OBJECT_PID=%d", source_pid,
                   "CODE_FILE=%s", file,
                   "CODE_LINE=%d", line,
                   "CODE_FUNC=%s", func,
                   "MESSAGE=%s", message,
                   /* custom fields below */
                   "FLATPAK_VERSION=" PACKAGE_VERSION,
                   "INSTALLATION=%s", installation,
                   "OPERATION=%s", change,
                   "REMOTE=%s", remote ? remote : "",
                   "REF=%s", ref ? ref : "",
                   "COMMIT=%s", commit ? commit : "",
                   "OLD_COMMIT=%s", old_commit ? old_commit : "",
                   "URL=%s", url ? url : "",
                   NULL);
#endif
}