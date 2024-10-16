int enc_untrusted_open(const char *pathname, int flags, ...) {
  int mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_open, pathname, TokLinuxFileStatusFlag(flags),
      TokLinuxFileModeFlag(mode));
}