ssize_t enc_untrusted_readlink(const char *pathname, char *buf, size_t bufsiz) {
  return static_cast<ssize_t>(EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_readlink, pathname, buf, bufsiz));
}