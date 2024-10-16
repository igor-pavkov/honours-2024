void __fastcall TSCPFileSystem::ChangeFileToken(const UnicodeString & DelimitedName,
  const TRemoteToken & Token, TFSCommand Cmd, const UnicodeString & RecursiveStr)
{
  UnicodeString Str;
  if (Token.IDValid)
  {
    Str = IntToStr(int(Token.ID));
  }
  else if (Token.NameValid)
  {
    Str = Token.Name;
  }

  if (!Str.IsEmpty())
  {
    ExecCommand(Cmd, ARRAYOFCONST((RecursiveStr, Str, DelimitedName)));
  }
}