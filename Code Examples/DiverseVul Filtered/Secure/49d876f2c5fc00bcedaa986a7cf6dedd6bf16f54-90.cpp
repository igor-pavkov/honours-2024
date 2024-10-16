UnicodeString __fastcall TCommandSet::FullCommand(TFSCommand Cmd, const TVarRec * args, int size)
{
  UnicodeString Separator;
  if (OneLineCommand[Cmd]) Separator = L" ; ";
    else Separator = L"\n";
  UnicodeString Line = Command(Cmd, args, size);
  UnicodeString LastLineCmd =
    Command(fsLastLine, ARRAYOFCONST((LastLine, ReturnVar)));
  UnicodeString FirstLineCmd;
  if (InteractiveCommand[Cmd])
    FirstLineCmd = Command(fsFirstLine, ARRAYOFCONST((FirstLine))) + Separator;

  UnicodeString Result;
  if (!Line.IsEmpty())
    Result = FORMAT(L"%s%s%s%s", (FirstLineCmd, Line, Separator, LastLineCmd));
  else
    Result = FORMAT(L"%s%s", (FirstLineCmd, LastLineCmd));
  return Result;
}