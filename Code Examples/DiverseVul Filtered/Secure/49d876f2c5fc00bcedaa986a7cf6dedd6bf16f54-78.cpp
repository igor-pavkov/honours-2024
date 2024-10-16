void __fastcall TSCPFileSystem::ReadCommandOutput(int Params, const UnicodeString * Cmd)
{
  try
  {
    if (Params & coWaitForLastLine)
    {
      UnicodeString Line;
      bool IsLast;
      unsigned int Total = 0;
      // #55: fixed so, even when last line of command output does not
      // contain CR/LF, we can recognize last line
      do
      {
        Line = FSecureShell->ReceiveLine();
        IsLast = IsLastLine(Line);
        if (!IsLast || !Line.IsEmpty())
        {
          FOutput->Add(Line);
          if (FLAGSET(Params, coReadProgress))
          {
            Total++;

            if (Total % 10 == 0)
            {
              bool Cancel; //dummy
              FTerminal->DoReadDirectoryProgress(Total, 0, Cancel);
            }
          }
        }
      }
      while (!IsLast);
    }
    if (Params & coRaiseExcept)
    {
      UnicodeString Message = FSecureShell->GetStdError();
      if ((Params & coExpectNoOutput) && FOutput->Count)
      {
        if (!Message.IsEmpty()) Message += L"\n";
        Message += FOutput->Text;
      }
      while (!Message.IsEmpty() && (Message.LastDelimiter(L"\n\r") == Message.Length()))
      {
        Message.SetLength(Message.Length() - 1);
      }

      bool WrongReturnCode =
        (ReturnCode > 1) || (ReturnCode == 1 && !(Params & coIgnoreWarnings));

      if (FOnCaptureOutput != NULL)
      {
        FOnCaptureOutput(IntToStr(ReturnCode), cotExitCode);
      }

      if (Params & coOnlyReturnCode && WrongReturnCode)
      {
        FTerminal->TerminalError(FMTLOAD(COMMAND_FAILED_CODEONLY, (ReturnCode)));
      }
      else if (!(Params & coOnlyReturnCode) &&
          ((!Message.IsEmpty() && ((FOutput->Count == 0) || !(Params & coIgnoreWarnings))) ||
           WrongReturnCode))
      {
        DebugAssert(Cmd != NULL);
        FTerminal->TerminalError(FMTLOAD(COMMAND_FAILED, (*Cmd, ReturnCode, Message)));
      }
    }
  }
  __finally
  {
    FProcessingCommand = false;
  }
}