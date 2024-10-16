void __fastcall TSCPFileSystem::SkipStartupMessage()
{
  try
  {
    FTerminal->LogEvent(L"Skipping host startup message (if any).");
    ExecCommand(fsNull, NULL, 0, 0);
  }
  catch (Exception & E)
  {
    FTerminal->CommandError(&E, LoadStr(SKIP_STARTUP_MESSAGE_ERROR), 0, HELP_SKIP_STARTUP_MESSAGE_ERROR);
  }
}