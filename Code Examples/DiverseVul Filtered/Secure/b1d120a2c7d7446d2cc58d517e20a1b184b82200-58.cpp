int enc_untrusted_inotify_add_watch(int fd, const char *pathname,
                                    uint32_t mask) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_inotify_add_watch, fd, pathname,
      TokLinuxInotifyEventMask(mask));
}