UnicodeString __fastcall TCommandSet::GetCommands(TFSCommand Cmd)
{
  CHECK_CMD;
  return CommandSet[Cmd].Command;
}