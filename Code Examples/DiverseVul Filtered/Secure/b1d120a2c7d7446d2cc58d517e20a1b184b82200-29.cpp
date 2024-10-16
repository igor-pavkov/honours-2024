int enc_untrusted_wait(int *wstatus) {
  int klinux_wstatus;
  pid_t ret = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_wait4, /*wpid=*/-1, &klinux_wstatus,
      /*options=*/0,
      /*rusage=*/nullptr);
  *wstatus = FromkLinuxToNewlibWstatus(klinux_wstatus);
  return ret;
}