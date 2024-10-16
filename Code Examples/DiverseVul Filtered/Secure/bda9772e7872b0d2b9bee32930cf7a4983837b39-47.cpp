inline void klinux_sigemptyset(klinux_sigset_t *klinux_set) {
  memset(klinux_set, 0, sizeof(klinux_sigset_t));
}