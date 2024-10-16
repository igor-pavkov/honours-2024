UnicodeString __fastcall TCommandSet::Command(TFSCommand Cmd, const TVarRec * args, int size)
{
  if (args) return Format(Commands[Cmd], args, size);
    else return Commands[Cmd];
}