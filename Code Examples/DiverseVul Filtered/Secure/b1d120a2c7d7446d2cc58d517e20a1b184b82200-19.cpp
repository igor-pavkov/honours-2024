void enc_untrusted_syslog(int priority, const char *message, int len) {
  EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_syslog,
                                      TokLinuxSyslogPriority(priority), message,
                                      len);
}