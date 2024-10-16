bool __fastcall TSCPFileSystem::RemoveLastLine(UnicodeString & Line,
    int & ReturnCode, UnicodeString LastLine)
{
  bool IsLastLine = false;
  if (LastLine.IsEmpty()) LastLine = LAST_LINE;
  // #55: fixed so, even when last line of command output does not
  // contain CR/LF, we can recognize last line
  int Pos = Line.Pos(LastLine);
  if (Pos)
  {
    // 2003-07-14: There must be nothing after return code number to
    // consider string as last line. This fixes bug with 'set' command
    // in console window
    UnicodeString ReturnCodeStr = Line.SubString(Pos + LastLine.Length() + 1,
      Line.Length() - Pos + LastLine.Length());
    if (TryStrToInt(ReturnCodeStr, ReturnCode))
    {
      IsLastLine = true;
      Line.SetLength(Pos - 1);
    }
  }
  return IsLastLine;
}