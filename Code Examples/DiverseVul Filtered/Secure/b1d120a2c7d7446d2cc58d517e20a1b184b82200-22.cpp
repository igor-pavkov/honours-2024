void enc_untrusted_openlog(const char *ident, int option, int facility) {
  MessageWriter input;
  MessageReader output;
  input.PushString(ident);
  input.Push<int>(TokLinuxSyslogOption(option));
  input.Push<int>(TokLinuxSyslogFacility(facility));

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kOpenLogHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_openlog", 1);
}