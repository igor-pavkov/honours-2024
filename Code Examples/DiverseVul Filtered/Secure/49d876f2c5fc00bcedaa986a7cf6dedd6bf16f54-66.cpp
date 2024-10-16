void __fastcall TCommandSet::SetCommands(TFSCommand Cmd, UnicodeString value)
{
  CHECK_CMD;
  wcscpy(CommandSet[Cmd].Command, value.SubString(1, MaxCommandLen - 1).c_str());
}