void __fastcall TSCPFileSystem::UnsetNationalVars()
{
  try
  {
    FTerminal->LogEvent(L"Clearing national user variables.");
    for (int Index = 0; Index < NationalVarCount; Index++)
    {
      ExecCommand(fsUnset, ARRAYOFCONST((NationalVars[Index])), false);
    }
  }
  catch (Exception &E)
  {
    FTerminal->CommandError(&E, LoadStr(UNSET_NATIONAL_ERROR));
  }
}