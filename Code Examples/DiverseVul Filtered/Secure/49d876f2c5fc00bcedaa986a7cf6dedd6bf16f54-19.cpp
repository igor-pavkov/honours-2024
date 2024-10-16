void __fastcall TSCPFileSystem::DetectUtf()
{
  switch (FTerminal->SessionData->NotUtf)
  {
    case asOn:
      FSecureShell->UtfStrings = false; // noop
      break;

    case asOff:
      FSecureShell->UtfStrings = true;
      break;

    default:
      DebugFail();
    case asAuto:
      FSecureShell->UtfStrings = false; // noop
      try
      {
        ExecCommand(fsLang, NULL, 0, false);

        if ((FOutput->Count >= 1) &&
            ContainsText(FOutput->Strings[0], L"UTF-8"))
        {
          FSecureShell->UtfStrings = true;
        }
      }
      catch (Exception & E)
      {
        // ignore non-fatal errors
        if (!FTerminal->Active)
        {
          throw;
        }
      }
      break;
  }

  if (FSecureShell->UtfStrings)
  {
    FTerminal->LogEvent(L"We will use UTF-8");
  }
  else
  {
    FTerminal->LogEvent(L"We will not use UTF-8");
  }
}