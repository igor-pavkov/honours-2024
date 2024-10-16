uint32_t enc_untrusted_sleep(uint32_t seconds) {
  MessageWriter input;
  input.Push<uint32_t>(seconds);
  MessageReader output;
  asylo::primitives::PrimitiveStatus status =
      asylo::host_call::NonSystemCallDispatcher(asylo::host_call::kSleepHandler,
                                                &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sleep", 2);

  // Returns sleep's return value directly since it doesn't set errno.
  return output.next<uint32_t>();
}