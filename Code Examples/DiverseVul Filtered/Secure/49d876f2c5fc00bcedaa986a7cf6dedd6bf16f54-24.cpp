UnicodeString __fastcall TCommandSet::ExtractCommand(UnicodeString Command)
{
  int P = Command.Pos(L" ");
  if (P > 0)
  {
    Command.SetLength(P-1);
  }
  return Command;
}