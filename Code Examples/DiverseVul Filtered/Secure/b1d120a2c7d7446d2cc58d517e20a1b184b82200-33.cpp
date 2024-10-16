int enc_untrusted_link(const char *oldpath, const char *newpath) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_link,
                                             oldpath, newpath);
}