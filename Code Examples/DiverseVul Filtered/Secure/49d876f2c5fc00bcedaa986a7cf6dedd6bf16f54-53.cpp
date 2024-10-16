void __fastcall TSCPFileSystem::CopyToLocal(TStrings * FilesToCopy,
  const UnicodeString TargetDir, const TCopyParamType * CopyParam,
  int Params, TFileOperationProgressType * OperationProgress,
  TOnceDoneOperation & OnceDoneOperation)
{
  bool CloseSCP = False;
  Params &= ~(cpAppend | cpResume);
  UnicodeString Options = L"";
  if (CopyParam->PreserveRights || CopyParam->PreserveTime) Options = L"-p";
  if (FTerminal->SessionData->Scp1Compatibility) Options += L" -1";

  FTerminal->LogEvent(FORMAT(L"Copying %d files/directories to local directory "
    "\"%s\"", (FilesToCopy->Count, TargetDir)));
  if (FTerminal->Configuration->ActualLogProtocol >= 0)
  {
    FTerminal->LogEvent(CopyParam->LogStr);
  }

  try
  {
    for (int IFile = 0; (IFile < FilesToCopy->Count) &&
      !OperationProgress->Cancel; IFile++)
    {
      UnicodeString FileName = FilesToCopy->Strings[IFile];
      TRemoteFile * File = (TRemoteFile *)FilesToCopy->Objects[IFile];
      DebugAssert(File);

      try
      {
        bool Success = true; // Have to be set to True (see ::SCPSink)
        SendCommand(FCommandSet->FullCommand(fsCopyToLocal,
          ARRAYOFCONST((Options, DelimitStr(FileName)))));
        SkipFirstLine();

        // Filename is used for error messaging and excluding files only
        // Send in full path to allow path-based excluding
        UnicodeString FullFileName = UnixExcludeTrailingBackslash(File->FullFileName);
        SCPSink(TargetDir, FullFileName, UnixExtractFilePath(FullFileName),
          CopyParam, Success, OperationProgress, Params, 0);
        // operation succeeded (no exception), so it's ok that
        // remote side closed SCP, but we continue with next file
        if (OperationProgress->Cancel == csRemoteAbort)
        {
          OperationProgress->SetCancel(csContinue);
        }

        // Move operation -> delete file/directory afterwards
        // but only if copying succeeded
        if ((Params & cpDelete) && Success && !OperationProgress->Cancel)
        {
          try
          {
            FTerminal->ExceptionOnFail = true;
            try
            {
              FILE_OPERATION_LOOP_BEGIN
              {
                // pass full file name in FileName, in case we are not moving
                // from current directory
                FTerminal->DeleteFile(FileName, File);
              }
              FILE_OPERATION_LOOP_END(FMTLOAD(DELETE_FILE_ERROR, (FileName)));
            }
            __finally
            {
              FTerminal->ExceptionOnFail = false;
            }
          }
          catch (EFatal &E)
          {
            throw;
          }
          catch (...)
          {
            // If user selects skip (or abort), nothing special actually occurs
            // we just run DoFinished with Success = False, so file won't
            // be deselected in panel (depends on assigned event handler)

            // On csCancel we would later try to close remote SCP, but it
            // is closed already
            if (OperationProgress->Cancel == csCancel)
            {
              OperationProgress->SetCancel(csRemoteAbort);
            }
            Success = false;
          }
        }

        FTerminal->OperationFinish(
          OperationProgress, File, FileName, (!OperationProgress->Cancel && Success), OnceDoneOperation);
      }
      catch (...)
      {
        FTerminal->OperationFinish(OperationProgress, File, FileName, false, OnceDoneOperation);
        CloseSCP = (OperationProgress->Cancel != csRemoteAbort);
        throw;
      }
    }
  }
  __finally
  {
    // In case that copying doesn't cause fatal error (ie. connection is
    // still active) but wasn't successful (exception or user termination)
    // we need to ensure, that SCP on remote side is closed
    if (FTerminal->Active && (CloseSCP ||
        (OperationProgress->Cancel == csCancel) ||
        (OperationProgress->Cancel == csCancelTransfer)))
    {
      bool LastLineRead;

      // If we get LastLine, it means that remote side 'scp' is already
      // terminated, so we need not to terminate it. There is also
      // possibility that remote side waits for confirmation, so it will hang.
      // This should not happen (hope)
      UnicodeString Line = FSecureShell->ReceiveLine();
      LastLineRead = IsLastLine(Line);
      if (!LastLineRead)
      {
        SCPSendError((OperationProgress->Cancel ? L"Terminated by user." : L"Exception"), true);
      }
      // Just in case, remote side already sent some more data (it's probable)
      // but we don't want to raise exception (user asked to terminate, it's not error)
      int ECParams = coOnlyReturnCode;
      if (!LastLineRead) ECParams |= coWaitForLastLine;
      ReadCommandOutput(ECParams);
    }
  }
}