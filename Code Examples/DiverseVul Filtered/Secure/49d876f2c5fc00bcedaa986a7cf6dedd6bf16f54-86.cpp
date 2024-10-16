void __fastcall TSCPFileSystem::ExecCommand(TFSCommand Cmd, const TVarRec * args,
  int size, int Params)
{
  if (Params < 0) Params = ecDefault;
  UnicodeString FullCommand = FCommandSet->FullCommand(Cmd, args, size);
  UnicodeString Command = FCommandSet->Command(Cmd, args, size);
  ExecCommand(FullCommand, Params, Command);
  if (Params & ecRaiseExcept)
  {
    Integer MinL = FCommandSet->MinLines[Cmd];
    Integer MaxL = FCommandSet->MaxLines[Cmd];
    if (((MinL >= 0) && (MinL > FOutput->Count)) ||
        ((MaxL >= 0) && (MaxL > FOutput->Count)))
    {
      FTerminal->TerminalError(FmtLoadStr(INVALID_OUTPUT_ERROR,
        ARRAYOFCONST((FullCommand, Output->Text))));
    }
  }
}