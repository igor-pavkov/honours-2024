void __fastcall TSCPFileSystem::SCPResponse(bool * GotLastLine)
{
  // Taken from scp.c response() and modified

  unsigned char Resp;
  FSecureShell->Receive(&Resp, 1);

  switch (Resp)
  {
    case 0:     /* ok */
      FTerminal->LogEvent(L"SCP remote side confirmation (0)");
      return;

    default:
    case 1:     /* error */
    case 2:     /* fatal error */
      // pscp adds 'Resp' to 'Msg', why?
      UnicodeString Msg = FSecureShell->ReceiveLine();
      UnicodeString Line = UnicodeString(static_cast<char>(Resp)) + Msg;
      if (IsLastLine(Line))
      {
        if (GotLastLine != NULL)
        {
          *GotLastLine = true;
        }

        /* TODO 1 : Show stderror to user? */
        FSecureShell->ClearStdError();

        try
        {
          ReadCommandOutput(coExpectNoOutput | coRaiseExcept | coOnlyReturnCode);
        }
        catch(...)
        {
          // when ReadCommandOutput() fails than remote SCP is terminated already
          if (GotLastLine != NULL)
          {
            *GotLastLine = true;
          }
          throw;
        }
      }
      else if (Resp == 1)
      {
        // While the OpenSSH scp client distinguishes the 1 for error and 2 for fatal errors,
        // the OpenSSH scp server always sends 1 even for fatal errors. Using the error message to tell
        // which errors are fatal and which are not.
        // This error list is valid for OpenSSH 5.3p1 and 7.2p2
        if (SameText(Msg, L"scp: ambiguous target") ||
            StartsText(L"scp: error: unexpected filename: ", Msg) ||
            StartsText(L"scp: protocol error: ", Msg))
        {
          FTerminal->LogEvent(L"SCP remote side error (1), fatal error detected from error message");
          Resp = 2;
          FScpFatalError = true;
        }
        else
        {
          FTerminal->LogEvent(L"SCP remote side error (1)");
        }
      }
      else
      {
        FTerminal->LogEvent(L"SCP remote side fatal error (2)");
        FScpFatalError = true;
      }

      if (Resp == 1)
      {
        throw EScpFileSkipped(NULL, Msg);
      }
      else
      {
        throw EScp(NULL, Msg);
      }
  }
}