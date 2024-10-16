void __fastcall TSCPFileSystem::CustomCommandOnFile(const UnicodeString FileName,
    const TRemoteFile * File, UnicodeString Command, int Params,
    TCaptureOutputEvent OutputEvent)
{
  DebugAssert(File);
  bool Dir = File->IsDirectory && FTerminal->CanRecurseToDirectory(File);
  if (Dir && (Params & ccRecursive))
  {
    TCustomCommandParams AParams;
    AParams.Command = Command;
    AParams.Params = Params;
    AParams.OutputEvent = OutputEvent;
    FTerminal->ProcessDirectory(FileName, FTerminal->CustomCommandOnFile,
      &AParams);
  }

  if (!Dir || (Params & ccApplyToDirectories))
  {
    TCustomCommandData Data(FTerminal);
    UnicodeString Cmd = TRemoteCustomCommand(
      Data, FTerminal->CurrentDirectory, FileName, L"").
      Complete(Command, true);

    if (!FTerminal->DoOnCustomCommand(Cmd))
    {
      AnyCommand(Cmd, OutputEvent);
    }
  }
}