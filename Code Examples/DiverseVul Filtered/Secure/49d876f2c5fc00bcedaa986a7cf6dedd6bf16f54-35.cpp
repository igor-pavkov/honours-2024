TStrings * __fastcall TCommandSet::CreateCommandList()
{
  TStrings * CommandList = new TStringList();
  for (Integer Index = 0; Index < ShellCommandCount; Index++)
  {
    UnicodeString Cmd = Commands[(TFSCommand)Index];
    if (!Cmd.IsEmpty())
    {
      Cmd = ExtractCommand(Cmd);
      if ((Cmd != L"%s") && (CommandList->IndexOf(Cmd) < 0))
        CommandList->Add(Cmd);
    }
  }
  return CommandList;
}