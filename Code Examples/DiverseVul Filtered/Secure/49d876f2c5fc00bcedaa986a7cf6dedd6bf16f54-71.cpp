void __fastcall TSCPFileSystem::SCPDirectorySource(const UnicodeString DirectoryName,
  const UnicodeString TargetDir, const TCopyParamType * CopyParam, int Params,
  TFileOperationProgressType * OperationProgress, int Level)
{
  int Attrs;

  FTerminal->LogEvent(FORMAT(L"Entering directory \"%s\".", (DirectoryName)));

  OperationProgress->SetFile(DirectoryName);
  UnicodeString DestFileName =
    FTerminal->ChangeFileName(
      CopyParam, ExtractFileName(DirectoryName), osLocal, Level == 0);

  // Get directory attributes
  FILE_OPERATION_LOOP_BEGIN
  {
    Attrs = FileGetAttrFix(ApiPath(DirectoryName));
    if (Attrs < 0) RaiseLastOSError();
  }
  FILE_OPERATION_LOOP_END(FMTLOAD(CANT_GET_ATTRS, (DirectoryName)));

  UnicodeString TargetDirFull = UnixIncludeTrailingBackslash(TargetDir + DestFileName);

  UnicodeString Buf;

  /* TODO 1: maybe send filetime */

  // Send directory modes (rights), filesize and file name
  Buf = FORMAT(L"D%s 0 %s",
    (CopyParam->RemoteFileRights(Attrs).Octal, DestFileName));
  FSecureShell->SendLine(Buf);
  SCPResponse();

  try
  {
    TSearchRecOwned SearchRec;
    bool FindOK = FTerminal->LocalFindFirstLoop(IncludeTrailingBackslash(DirectoryName) + L"*.*", SearchRec);

    while (FindOK && !OperationProgress->Cancel)
    {
      UnicodeString FileName = IncludeTrailingBackslash(DirectoryName) + SearchRec.Name;
      try
      {
        if (SearchRec.IsRealFile())
        {
          SCPSource(FileName, TargetDirFull, CopyParam, Params, OperationProgress, Level + 1);
        }
      }
      // Previously we caught ESkipFile, making error being displayed
      // even when file was excluded by mask. Now the ESkipFile is special
      // case without error message.
      catch (EScpFileSkipped &E)
      {
        TQueryParams Params(qpAllowContinueOnError);
        TSuspendFileOperationProgress Suspend(OperationProgress);

        if (FTerminal->QueryUserException(FMTLOAD(COPY_ERROR, (FileName)), &E,
              qaOK | qaAbort, &Params, qtError) == qaAbort)
        {
          OperationProgress->SetCancel(csCancel);
        }
        if (!FTerminal->HandleException(&E))
        {
          throw;
        }
      }
      catch (ESkipFile &E)
      {
        // If ESkipFile occurs, just log it and continue with next file
        TSuspendFileOperationProgress Suspend(OperationProgress);
        if (!FTerminal->HandleException(&E))
        {
          throw;
        }
      }

      FindOK = FTerminal->LocalFindNextLoop(SearchRec);
    }

    SearchRec.Close();

    /* TODO : Delete also read-only directories. */
    /* TODO : Show error message on failure. */
    if (!OperationProgress->Cancel)
    {
      if (FLAGSET(Params, cpDelete))
      {
        RemoveDir(ApiPath(DirectoryName));
      }
      else if (CopyParam->ClearArchive && FLAGSET(Attrs, faArchive))
      {
        FILE_OPERATION_LOOP_BEGIN
        {
          THROWOSIFFALSE(FileSetAttr(ApiPath(DirectoryName), Attrs & ~faArchive) == 0);
        }
        FILE_OPERATION_LOOP_END(FMTLOAD(CANT_SET_ATTRS, (DirectoryName)));
      }
    }
  }
  __finally
  {
    if (FTerminal->Active)
    {
      // Tell remote side, that we're done.
      FTerminal->LogEvent(FORMAT(L"Leaving directory \"%s\".", (DirectoryName)));
      FSecureShell->SendLine(L"E");
      SCPResponse();
    }
  }
}