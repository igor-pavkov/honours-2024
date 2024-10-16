void __fastcall TSCPFileSystem::Idle()
{
  // Keep session alive
  if ((FTerminal->SessionData->PingType != ptOff) &&
      (Now() - FSecureShell->LastDataSent > FTerminal->SessionData->PingIntervalDT))
  {
    if ((FTerminal->SessionData->PingType == ptDummyCommand) &&
        FSecureShell->Ready)
    {
      if (!FProcessingCommand)
      {
        ExecCommand(fsNull, NULL, 0, 0);
      }
      else
      {
        FTerminal->LogEvent(L"Cannot send keepalive, command is being executed");
        // send at least SSH-level keepalive, if nothing else, it at least updates
        // LastDataSent, no the next keepalive attempt is postponed
        FSecureShell->KeepAlive();
      }
    }
    else
    {
      FSecureShell->KeepAlive();
    }
  }

  FSecureShell->Idle();
}