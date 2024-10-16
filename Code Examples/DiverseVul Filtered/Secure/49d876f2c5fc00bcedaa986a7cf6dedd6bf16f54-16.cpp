void __fastcall TSCPFileSystem::AnyCommand(const UnicodeString Command,
  TCaptureOutputEvent OutputEvent)
{
  DebugAssert(FSecureShell->OnCaptureOutput == NULL);
  if (OutputEvent != NULL)
  {
    FSecureShell->OnCaptureOutput = CaptureOutput;
    FOnCaptureOutput = OutputEvent;
  }

  try
  {
    ExecCommand(fsAnyCommand, ARRAYOFCONST((Command)),
      ecDefault | ecIgnoreWarnings);
  }
  __finally
  {
    FOnCaptureOutput = NULL;
    FSecureShell->OnCaptureOutput = NULL;
  }
}