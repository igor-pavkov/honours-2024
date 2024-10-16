void __fastcall TSCPFileSystem::CaptureOutput(const UnicodeString & AddedLine, TCaptureOutputType OutputType)
{
  int ReturnCode;
  UnicodeString Line = AddedLine;
  // TSecureShell never uses cotExitCode
  DebugAssert((OutputType == cotOutput) || (OutputType == cotError));
  if ((OutputType == cotError) || DebugAlwaysFalse(OutputType == cotExitCode) ||
      !RemoveLastLine(Line, ReturnCode) ||
      !Line.IsEmpty())
  {
    DebugAssert(FOnCaptureOutput != NULL);
    FOnCaptureOutput(Line, OutputType);
  }
}