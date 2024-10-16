void __fastcall TSCPFileSystem::DetectReturnVar()
{
  // This suppose that something was already executed (probably SkipStartupMessage())
  // or return code variable is already set on start up.

  try
  {
    // #60 17.10.01: "status" and "?" switched
    UnicodeString ReturnVars[2] = { L"status", L"?" };
    UnicodeString NewReturnVar = L"";
    FTerminal->LogEvent(L"Detecting variable containing return code of last command.");
    for (int Index = 0; Index < 2; Index++)
    {
      bool Success = true;

      try
      {
        FTerminal->LogEvent(FORMAT(L"Trying \"$%s\".", (ReturnVars[Index])));
        ExecCommand(fsVarValue, ARRAYOFCONST((ReturnVars[Index])));
        if ((Output->Count != 1) || (StrToIntDef(Output->Strings[0], 256) > 255))
        {
          FTerminal->LogEvent(L"The response is not numerical exit code");
          Abort();
        }
      }
      catch (EFatal &E)
      {
        // if fatal error occurs, we need to exit ...
        throw;
      }
      catch (Exception &E)
      {
        // ...otherwise, we will try next variable (if any)
        Success = false;
      }

      if (Success)
      {
        NewReturnVar = ReturnVars[Index];
        break;
      }
    }

    if (NewReturnVar.IsEmpty())
    {
      EXCEPTION;
    }
    else
    {
      FCommandSet->ReturnVar = NewReturnVar;
      FTerminal->LogEvent(FORMAT(L"Return code variable \"%s\" selected.",
        (FCommandSet->ReturnVar)));
    }
  }
  catch (Exception &E)
  {
    FTerminal->CommandError(&E, LoadStr(DETECT_RETURNVAR_ERROR));
  }
}