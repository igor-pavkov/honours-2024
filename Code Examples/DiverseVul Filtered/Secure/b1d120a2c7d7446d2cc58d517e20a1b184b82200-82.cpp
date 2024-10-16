int enc_untrusted_rename(const char *oldpath, const char *newpath) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_rename,
                                             oldpath, newpath);
}