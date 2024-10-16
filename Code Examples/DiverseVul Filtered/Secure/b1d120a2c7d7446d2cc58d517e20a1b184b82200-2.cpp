int enc_untrusted_lstat(const char *pathname, struct stat *statbuf) {
  struct klinux_stat stat_kernel;
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_lstat, pathname, &stat_kernel);

  if (FromkLinuxStat(&stat_kernel, statbuf)) {
    statbuf->st_mode = FromkLinuxFileModeFlag(stat_kernel.klinux_st_mode);
  }
  return result;
}