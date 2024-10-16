__fastcall TCommandSet::TCommandSet(TSessionData *aSessionData):
  FSessionData(aSessionData), FReturnVar(L"")
{
  DebugAssert(FSessionData);
  Default();
}