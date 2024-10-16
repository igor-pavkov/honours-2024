void __fastcall TSCPFileSystem::ClearAliases()
{
  try
  {
    FTerminal->LogEvent(L"Clearing all aliases.");
    ClearAlias(TCommandSet::ExtractCommand(FTerminal->SessionData->ListingCommand));
    TStrings * CommandList = FCommandSet->CreateCommandList();
    try
    {
      for (int Index = 0; Index < CommandList->Count; Index++)
      {
        ClearAlias(CommandList->Strings[Index]);
      }
    }
    __finally
    {
      delete CommandList;
    }
  }
  catch (Exception &E)
  {
    FTerminal->CommandError(&E, LoadStr(UNALIAS_ALL_ERROR));
  }
}