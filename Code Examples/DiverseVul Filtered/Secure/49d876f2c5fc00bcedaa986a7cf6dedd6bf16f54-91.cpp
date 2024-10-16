UnicodeString __fastcall TCommandSet::GetReturnVar()
{
  DebugAssert(SessionData);
  if (!FReturnVar.IsEmpty())
  {
    return UnicodeString(L'$') + FReturnVar;
  }
  else if (SessionData->DetectReturnVar)
  {
    return L'0';
  }
  else
  {
    return UnicodeString(L'$') + SessionData->ReturnVar;
  }
}