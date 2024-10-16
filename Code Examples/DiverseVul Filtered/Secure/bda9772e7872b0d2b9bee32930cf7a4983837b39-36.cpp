int64_t TokLinuxStatFsFlags(int64_t input) {
  int64_t result = 0;

  if (input & ST_NOSUID) result |= kLinux_ST_NOSUID;
  if (input & ST_RDONLY) result |= kLinux_ST_RDONLY;
#if (defined(__USE_GNU) && __USE_GNU) || \
    (defined(__GNU_VISIBLE) && __GNU_VISIBLE)
  if (input & ST_MANDLOCK) result |= kLinux_ST_MANDLOCK;
  if (input & ST_NOATIME) result |= kLinux_ST_NOATIME;
  if (input & ST_NODEV) result |= kLinux_ST_NODEV;
  if (input & ST_NODIRATIME) result |= kLinux_ST_NODIRATIME;
  if (input & ST_NOEXEC) result |= kLinux_ST_NOEXEC;
  if (input & ST_RELATIME) result |= kLinux_ST_RELATIME;
  if (input & ST_SYNCHRONOUS) result |= kLinux_ST_SYNCHRONOUS;
#endif
  return result;
}