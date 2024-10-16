void __fastcall TSCPFileSystem::CopyToRemote(TStrings * FilesToCopy,
  const UnicodeString TargetDir, const TCopyParamType * CopyParam,
  int Params, TFileOperationProgressType * OperationProgress,
  TOnceDoneOperation & OnceDoneOperation)
{
  // scp.c: source(), toremote()
  DebugAssert(FilesToCopy && OperationProgress);

  Params &= ~(cpAppend | cpResume);
  UnicodeString Options = L"";
  bool CheckExistence = UnixSamePath(TargetDir, FTerminal->CurrentDirectory) &&
    (FTerminal->FFiles != NULL) && FTerminal->FFiles->Loaded;
  bool CopyBatchStarted = false;
  bool Failed = true;
  bool GotLastLine = false;

  UnicodeString TargetDirFull = UnixIncludeTrailingBackslash(TargetDir);

  if (CopyParam->PreserveRights) Options = L"-p";
  if (FTerminal->SessionData->Scp1Compatibility) Options += L" -1";

  FScpFatalError = false;
  SendCommand(FCommandSet->FullCommand(fsCopyToRemote,
    ARRAYOFCONST((Options, DelimitStr(UnixExcludeTrailingBackslash(TargetDir))))));
  SkipFirstLine();

  try
  {
    try
    {
      SCPResponse(&GotLastLine);

      // This can happen only if SCP command is not executed and return code is 0
      // It has never happened to me (return code is usually 127)
      if (GotLastLine)
      {
        throw Exception(L"");
      }
    }
    catch(Exception & E)
    {
      if (GotLastLine && FTerminal->Active)
      {
        FTerminal->TerminalError(&E, LoadStr(SCP_INIT_ERROR));
      }
      else
      {
        throw;
      }
    }
    CopyBatchStarted = true;

    for (int IFile = 0; (IFile < FilesToCopy->Count) &&
      !OperationProgress->Cancel; IFile++)
    {
      UnicodeString FileName = FilesToCopy->Strings[IFile];
      bool CanProceed;

      UnicodeString FileNameOnly =
        FTerminal->ChangeFileName(
          CopyParam, ExtractFileName(FileName), osLocal, true);

      if (CheckExistence)
      {
        // previously there was assertion on FTerminal->FFiles->Loaded, but it
        // fails for scripting, if 'ls' is not issued before.
        // formally we should call CheckRemoteFile here but as checking is for
        // free here (almost) ...
        TRemoteFile * File = FTerminal->FFiles->FindFile(FileNameOnly);
        if (File != NULL)
        {
          unsigned int Answer;
          if (File->IsDirectory)
          {
            UnicodeString Message = FMTLOAD(DIRECTORY_OVERWRITE, (FileNameOnly));
            TQueryParams QueryParams(qpNeverAskAgainCheck);

            TSuspendFileOperationProgress Suspend(OperationProgress);
            Answer = FTerminal->ConfirmFileOverwrite(
              FileName, FileNameOnly, NULL,
              qaYes | qaNo | qaCancel | qaYesToAll | qaNoToAll,
              &QueryParams, osRemote, CopyParam, Params, OperationProgress, Message);
          }
          else
          {
            __int64 MTime;
            TOverwriteFileParams FileParams;
            FTerminal->OpenLocalFile(FileName, GENERIC_READ,
              NULL, NULL, NULL, &MTime, NULL,
              &FileParams.SourceSize);
            FileParams.SourceTimestamp = UnixToDateTime(MTime,
              FTerminal->SessionData->DSTMode);
            FileParams.DestSize = File->Size;
            FileParams.DestTimestamp = File->Modification;

            Answer = ConfirmOverwrite(FileName, FileNameOnly, osRemote,
              &FileParams, CopyParam, Params, OperationProgress);
          }

          switch (Answer)
          {
            case qaYes:
              CanProceed = true;
              break;

            case qaCancel:
              OperationProgress->SetCancelAtLeast(csCancel);
            case qaNo:
              CanProceed = false;
              break;

            default:
              DebugFail();
              break;
          }
        }
        else
        {
          CanProceed = true;
        }
      }
      else
      {
        CanProceed = true;
      }

      if (CanProceed)
      {
        if (FTerminal->SessionData->CacheDirectories)
        {
          FTerminal->DirectoryModified(TargetDir, false);

          if (DirectoryExists(ApiPath(FileName)))
          {
            FTerminal->DirectoryModified(UnixIncludeTrailingBackslash(TargetDir)+
              FileNameOnly, true);
          }
        }

        void * Item = static_cast<void *>(FilesToCopy->Objects[IFile]);

        try
        {
          SCPSource(FileName, TargetDirFull,
            CopyParam, Params, OperationProgress, 0);
          FTerminal->OperationFinish(OperationProgress, Item, FileName, true, OnceDoneOperation);
        }
        catch (EScpFileSkipped &E)
        {
          TQueryParams Params(qpAllowContinueOnError);

          TSuspendFileOperationProgress Suspend(OperationProgress);

          if (FTerminal->QueryUserException(FMTLOAD(COPY_ERROR, (FileName)), &E,
            qaOK | qaAbort, &Params, qtError) == qaAbort)
          {
            OperationProgress->SetCancel(csCancel);
          }
          FTerminal->OperationFinish(OperationProgress, Item, FileName, false, OnceDoneOperation);
          if (!FTerminal->HandleException(&E))
          {
            throw;
          }
        }
        catch (ESkipFile &E)
        {
          FTerminal->OperationFinish(OperationProgress, Item, FileName, false, OnceDoneOperation);

          {
            TSuspendFileOperationProgress Suspend(OperationProgress);
            // If ESkipFile occurs, just log it and continue with next file
            if (!FTerminal->HandleException(&E))
            {
              throw;
            }
          }
        }
        catch (...)
        {
          FTerminal->OperationFinish(OperationProgress, Item, FileName, false, OnceDoneOperation);
          throw;
        }
      }
    }
    Failed = false;
  }
  __finally
  {
    // Tell remote side, that we're done.
    if (FTerminal->Active)
    {
      try
      {
        if (!GotLastLine)
        {
          if (CopyBatchStarted && !FScpFatalError)
          {
            // What about case, remote side sends fatal error ???
            // (Not sure, if it causes remote side to terminate scp)
            FSecureShell->SendLine(L"E");
            SCPResponse();
          }
          /* TODO 1 : Show stderror to user? */
          FSecureShell->ClearStdError();

          ReadCommandOutput(coExpectNoOutput | coWaitForLastLine | coOnlyReturnCode |
            (Failed ? 0 : coRaiseExcept));
        }
      }
      catch (Exception &E)
      {
        // Only log error message (it should always succeed, but
        // some pending error maybe in queue) }
        FTerminal->Log->AddException(&E);
      }
    }
  }
}