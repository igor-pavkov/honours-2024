int64_t FromkLinuxStatFsFlags(int64_t input) {
  int64_t result = 0;

  if (input & kLinux_ST_NOSUID) result |= ST_NOSUID;
  if (input & kLinux_ST_RDONLY) result |= ST_RDONLY;
#if (defined(__USE_GNU) && __USE_GNU) || \
    (defined(__GNU_VISIBLE) && __GNU_VISIBLE)
  if (input & kLinux_ST_MANDLOCK) result |= ST_MANDLOCK;
  if (input & kLinux_ST_NOATIME) result |= ST_NOATIME;
  if (input & kLinux_ST_NODEV) result |= ST_NODEV;
  if (input & kLinux_ST_NODIRATIME) result |= ST_NODIRATIME;
  if (input & kLinux_ST_NOEXEC) result |= ST_NOEXEC;
  if (input & kLinux_ST_RELATIME) result |= ST_RELATIME;
  if (input & kLinux_ST_SYNCHRONOUS) result |= ST_SYNCHRONOUS;
#endif
  return result;
}