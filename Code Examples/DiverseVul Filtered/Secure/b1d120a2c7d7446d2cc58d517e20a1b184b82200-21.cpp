void enc_untrusted_hex_dump(const void *buf, size_t nbytes) {
  MessageWriter input;
  MessageReader output;
  input.PushByReference(Extent{reinterpret_cast<const char *>(buf), nbytes});
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kHexDumpHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_hex_dump", 2);
}