int enc_untrusted_fstat(int fd, struct stat *statbuf) {
  struct klinux_stat stat_kernel;
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_fstat, fd, &stat_kernel);

  if (FromkLinuxStat(&stat_kernel, statbuf)) {
    statbuf->st_mode = FromkLinuxFileModeFlag(stat_kernel.klinux_st_mode);
  }
  return result;
}