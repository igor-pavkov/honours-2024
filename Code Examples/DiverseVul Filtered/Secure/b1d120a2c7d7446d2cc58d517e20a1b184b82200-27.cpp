int enc_untrusted_inotify_init1(int flags) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_inotify_init1, TokLinuxInotifyFlag(flags));
}