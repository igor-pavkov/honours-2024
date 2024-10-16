void __fastcall TSCPFileSystem::SCPSource(const UnicodeString FileName,
  const UnicodeString TargetDir, const TCopyParamType * CopyParam, int Params,
  TFileOperationProgressType * OperationProgress, int Level)
{
  UnicodeString DestFileName =
    FTerminal->ChangeFileName(
      CopyParam, ExtractFileName(FileName), osLocal, Level == 0);

  FTerminal->LogEvent(FORMAT(L"File: \"%s\"", (FileName)));

  OperationProgress->SetFile(FileName, false);

  if (!FTerminal->AllowLocalFileTransfer(FileName, CopyParam, OperationProgress))
  {
    throw ESkipFile();
  }

  TLocalFileHandle Handle;
  FTerminal->OpenLocalFile(FileName, GENERIC_READ, Handle);

  OperationProgress->SetFileInProgress();

  if (Handle.Directory)
  {
    SCPDirectorySource(FileName, TargetDir, CopyParam, Params, OperationProgress, Level);
  }
  else
  {
    UnicodeString AbsoluteFileName = FTerminal->AbsolutePath(TargetDir + DestFileName, false);

    DebugAssert(Handle.Handle);
    std::unique_ptr<TStream> Stream(new TSafeHandleStream((THandle)Handle.Handle));

    // File is regular file (not directory)
    FTerminal->LogEvent(FORMAT(L"Copying \"%s\" to remote directory started.", (FileName)));

    OperationProgress->SetLocalSize(Handle.Size);

    // Suppose same data size to transfer as to read
    // (not true with ASCII transfer)
    OperationProgress->SetTransferSize(OperationProgress->LocalSize);
    OperationProgress->SetTransferringFile(false);

    FTerminal->SelectSourceTransferMode(Handle, CopyParam);

    TUploadSessionAction Action(FTerminal->ActionLog);
    Action.FileName(ExpandUNCFileName(FileName));
    Action.Destination(AbsoluteFileName);

    TRights Rights = CopyParam->RemoteFileRights(Handle.Attrs);

    try
    {
      // During ASCII transfer we will load whole file to this buffer
      // than convert EOL and send it at once, because before converting EOL
      // we can't know its size
      TFileBuffer AsciiBuf;
      bool ConvertToken = false;
      do
      {
        // Buffer for one block of data
        TFileBuffer BlockBuf;

        // This is crucial, if it fails during file transfer, it's fatal error
        FILE_OPERATION_LOOP_BEGIN
        {
          BlockBuf.LoadStream(Stream.get(), OperationProgress->LocalBlockSize(), true);
        }
        FILE_OPERATION_LOOP_END_EX(
          FMTLOAD(READ_ERROR, (FileName)),
          FLAGMASK(!OperationProgress->TransferringFile, folAllowSkip));

        OperationProgress->AddLocallyUsed(BlockBuf.Size);

        // We do ASCII transfer: convert EOL of current block
        // (we don't convert whole buffer, cause it would produce
        // huge memory-transfers while inserting/deleting EOL characters)
        // Than we add current block to file buffer
        if (OperationProgress->AsciiTransfer)
        {
          int ConvertParams =
            FLAGMASK(CopyParam->RemoveCtrlZ, cpRemoveCtrlZ) |
            FLAGMASK(CopyParam->RemoveBOM, cpRemoveBOM);
          BlockBuf.Convert(FTerminal->Configuration->LocalEOLType,
            FTerminal->SessionData->EOLType,
            ConvertParams, ConvertToken);
          BlockBuf.Memory->Seek(0, soFromBeginning);
          AsciiBuf.ReadStream(BlockBuf.Memory, BlockBuf.Size, true);
          // We don't need it any more
          BlockBuf.Memory->Clear();
          // Calculate total size to sent (assume that ratio between
          // size of source and size of EOL-transformed data would remain same)
          // First check if file contains anything (div by zero!)
          if (OperationProgress->LocallyUsed)
          {
            __int64 X = OperationProgress->LocalSize;
            X *= AsciiBuf.Size;
            X /= OperationProgress->LocallyUsed;
            OperationProgress->ChangeTransferSize(X);
          }
            else
          {
            OperationProgress->ChangeTransferSize(0);
          }
        }

        // We send file information on first pass during BINARY transfer
        // and on last pass during ASCII transfer
        // BINARY: We succeeded reading first buffer from file, hopefully
        // we will be able to read whole, so we send file info to remote side
        // This is done, because when reading fails we can't interrupt sending
        // (don't know how to tell other side that it failed)
        if (!OperationProgress->TransferringFile &&
            (!OperationProgress->AsciiTransfer || OperationProgress->IsLocallyDone()))
        {
          UnicodeString Buf;

          if (CopyParam->PreserveTime)
          {
            // Send last file access and modification time
            // TVarRec don't understand 'unsigned int' -> we use sprintf()
            Buf.sprintf(L"T%lu 0 %lu 0", static_cast<unsigned long>(Handle.MTime),
              static_cast<unsigned long>(Handle.ATime));
            FSecureShell->SendLine(Buf);
            SCPResponse();
          }

          // Send file modes (rights), filesize and file name
          // TVarRec don't understand 'unsigned int' -> we use sprintf()
          Buf.sprintf(L"C%s %Ld %s",
            Rights.Octal.data(),
            (OperationProgress->AsciiTransfer ? (__int64)AsciiBuf.Size :
              OperationProgress->LocalSize),
            DestFileName.data());
          FSecureShell->SendLine(Buf);
          SCPResponse();
          // Indicate we started transferring file, we need to finish it
          // If not, it's fatal error
          OperationProgress->SetTransferringFile(true);

          // If we're doing ASCII transfer, this is last pass
          // so we send whole file
          /* TODO : We can't send file above 32bit size in ASCII mode! */
          if (OperationProgress->AsciiTransfer)
          {
            FTerminal->LogEvent(FORMAT(L"Sending ASCII data (%u bytes)",
              (AsciiBuf.Size)));
            // Should be equal, just in case it's rounded (see above)
            OperationProgress->ChangeTransferSize(AsciiBuf.Size);
            while (!OperationProgress->IsTransferDone())
            {
              unsigned long BlockSize = OperationProgress->TransferBlockSize();
              FSecureShell->Send(
                reinterpret_cast<unsigned char *>(AsciiBuf.Data + (unsigned int)OperationProgress->TransferredSize),
                BlockSize);
              OperationProgress->AddTransferred(BlockSize);
              if (OperationProgress->Cancel == csCancelTransfer)
              {
                throw Exception(MainInstructions(LoadStr(USER_TERMINATED)));
              }
            }
          }
        }

        // At end of BINARY transfer pass, send current block
        if (!OperationProgress->AsciiTransfer)
        {
          if (!OperationProgress->TransferredSize)
          {
            FTerminal->LogEvent(FORMAT(L"Sending BINARY data (first block, %u bytes)",
              (BlockBuf.Size)));
          }
          else if (FTerminal->Configuration->ActualLogProtocol >= 1)
          {
            FTerminal->LogEvent(FORMAT(L"Sending BINARY data (%u bytes)",
              (BlockBuf.Size)));
          }
          FSecureShell->Send(reinterpret_cast<const unsigned char *>(BlockBuf.Data), BlockBuf.Size);
          OperationProgress->AddTransferred(BlockBuf.Size);
        }

        if ((OperationProgress->Cancel == csCancelTransfer) ||
            (OperationProgress->Cancel == csCancel && !OperationProgress->TransferringFile))
        {
          throw Exception(MainInstructions(LoadStr(USER_TERMINATED)));
        }
      }
      while (!OperationProgress->IsLocallyDone() || !OperationProgress->IsTransferDone());

      FSecureShell->SendNull();
      try
      {
        SCPResponse();
        // If one of two following exceptions occurs, it means, that remote
        // side already know, that file transfer finished, even if it failed
        // so we don't have to throw EFatal
      }
      catch (EScp &E)
      {
        // SCP protocol fatal error
        OperationProgress->SetTransferringFile(false);
        throw;
      }
      catch (EScpFileSkipped &E)
      {
        // SCP protocol non-fatal error
        OperationProgress->SetTransferringFile(false);
        throw;
      }

      // We succeeded transferring file, from now we can handle exceptions
      // normally -> no fatal error
      OperationProgress->SetTransferringFile(false);
    }
    catch (Exception &E)
    {
      // EScpFileSkipped is derived from ESkipFile,
      // but is does not indicate file skipped by user here
      if (dynamic_cast<EScpFileSkipped *>(&E) != NULL)
      {
        Action.Rollback(&E);
      }
      else
      {
        FTerminal->RollbackAction(Action, OperationProgress, &E);
      }

      // Every exception during file transfer is fatal
      if (OperationProgress->TransferringFile)
      {
        FTerminal->FatalError(&E, FMTLOAD(COPY_FATAL, (FileName)));
      }
      else
      {
        throw;
      }
    }

    // With SCP we are not able to distinguish reason for failure
    // (upload itself, touch or chmod).
    // So we always report error with upload action and
    // log touch and chmod actions only if upload succeeds.
    if (CopyParam->PreserveTime)
    {
      TTouchSessionAction(FTerminal->ActionLog, AbsoluteFileName, Handle.Modification);
    }
    if (CopyParam->PreserveRights)
    {
      TChmodSessionAction(FTerminal->ActionLog, AbsoluteFileName,
        Rights);
    }

    FTerminal->LogFileDone(OperationProgress, AbsoluteFileName);
    // Stream is disposed here
  }

  Handle.Release();

  FTerminal->UpdateSource(Handle, CopyParam, Params);

  FTerminal->LogEvent(FORMAT(L"Copying \"%s\" to remote directory finished.", (FileName)));
}