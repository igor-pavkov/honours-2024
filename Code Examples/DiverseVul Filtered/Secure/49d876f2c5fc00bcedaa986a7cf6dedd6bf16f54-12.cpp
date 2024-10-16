void __fastcall TSCPFileSystem::DoStartup()
{
  // Capabilities of SCP protocol are fixed
  FTerminal->SaveCapabilities(FFileSystemInfo);

  // SkipStartupMessage and DetectReturnVar must succeed,
  // otherwise session is to be closed.
  try
  {
    FTerminal->ExceptionOnFail = true;
    SkipStartupMessage();
    if (FTerminal->SessionData->DetectReturnVar) DetectReturnVar();
    FTerminal->ExceptionOnFail = false;
  }
  catch (Exception & E)
  {
    FTerminal->FatalError(&E, L"");
  }

  // Needs to be done before UnsetNationalVars()
  DetectUtf();

  #define COND_OPER(OPER) if (FTerminal->SessionData->OPER) OPER()
  COND_OPER(ClearAliases);
  COND_OPER(UnsetNationalVars);
  #undef COND_OPER
}