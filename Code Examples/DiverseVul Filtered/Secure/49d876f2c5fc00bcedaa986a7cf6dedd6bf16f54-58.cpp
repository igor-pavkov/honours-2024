void __fastcall TSCPFileSystem::ExecCommand(const UnicodeString & Cmd, int Params,
  const UnicodeString & CmdString)
{
  if (Params < 0)
  {
    Params = ecDefault;
  }

  TOperationVisualizer Visualizer(FTerminal->UseBusyCursor);

  SendCommand(Cmd);

  int COParams = coWaitForLastLine;
  if (Params & ecRaiseExcept) COParams |= coRaiseExcept;
  if (Params & ecIgnoreWarnings) COParams |= coIgnoreWarnings;
  if (Params & ecReadProgress) COParams |= coReadProgress;
  ReadCommandOutput(COParams, &CmdString);
}