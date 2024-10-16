void __fastcall TSCPFileSystem::ReadDirectory(TRemoteFileList * FileList)
{
  DebugAssert(FileList);
  // emptying file list moved before command execution
  FileList->Reset();

  bool Again;

  do
  {
    Again = false;
    try
    {
      int Params = ecDefault | ecReadProgress |
        FLAGMASK(FTerminal->SessionData->IgnoreLsWarnings, ecIgnoreWarnings);
      const wchar_t * Options =
        ((FLsFullTime == asAuto) || (FLsFullTime == asOn)) ? FullTimeOption : L"";
      bool ListCurrentDirectory = (FileList->Directory == FTerminal->CurrentDirectory);
      if (ListCurrentDirectory)
      {
        FTerminal->LogEvent(L"Listing current directory.");
        ExecCommand(fsListCurrentDirectory,
          ARRAYOFCONST((FTerminal->SessionData->ListingCommand, Options)), Params);
      }
      else
      {
        FTerminal->LogEvent(FORMAT(L"Listing directory \"%s\".",
          (FileList->Directory)));
        ExecCommand(fsListDirectory,
          ARRAYOFCONST((FTerminal->SessionData->ListingCommand, Options,
            DelimitStr(FileList->Directory))),
          Params);
      }

      TRemoteFile * File;

      // If output is not empty, we have successfully got file listing,
      // otherwise there was an error, in case it was "permission denied"
      // we try to get at least parent directory (see "else" statement below)
      if (FOutput->Count > 0)
      {
        // Copy LS command output, because eventual symlink analysis would
        // modify FTerminal->Output
        TStringList * OutputCopy = new TStringList();
        try
        {
          OutputCopy->Assign(FOutput);

          // delete leading "total xxx" line
          // On some hosts there is not "total" but "totalt". What's the reason??
          // see mail from "Jan Wiklund (SysOp)" <jan@park.se>
          if (IsTotalListingLine(OutputCopy->Strings[0]))
          {
            OutputCopy->Delete(0);
          }

          for (int Index = 0; Index < OutputCopy->Count; Index++)
          {
            UnicodeString OutputLine = OutputCopy->Strings[Index];
            if (!OutputLine.IsEmpty())
            {
              File = CreateRemoteFile(OutputCopy->Strings[Index]);
              FileList->AddFile(File);
            }
          }
        }
        __finally
        {
          delete OutputCopy;
        }
      }
      else
      {
        bool Empty;
        if (ListCurrentDirectory)
        {
          // Empty file list -> probably "permission denied", we
          // at least get link to parent directory ("..")
          FTerminal->ReadFile(
            UnixIncludeTrailingBackslash(FTerminal->FFiles->Directory) +
              PARENTDIRECTORY, File);
          Empty = (File == NULL);
          if (!Empty)
          {
            DebugAssert(File->IsParentDirectory);
            FileList->AddFile(File);
          }
        }
        else
        {
          Empty = true;
        }

        if (Empty)
        {
          throw ExtException(
            NULL, FMTLOAD(EMPTY_DIRECTORY, (FileList->Directory)),
            HELP_EMPTY_DIRECTORY);
        }
      }

      if (FLsFullTime == asAuto)
      {
          FTerminal->LogEvent(
            FORMAT(L"Directory listing with %s succeed, next time all errors during "
              "directory listing will be displayed immediately.",
              (FullTimeOption)));
          FLsFullTime = asOn;
      }
    }
    catch(Exception & E)
    {
      if (FTerminal->Active)
      {
        if (FLsFullTime == asAuto)
        {
          FTerminal->Log->AddException(&E);
          FLsFullTime = asOff;
          Again = true;
          FTerminal->LogEvent(
            FORMAT(L"Directory listing with %s failed, try again regular listing.",
            (FullTimeOption)));
        }
        else
        {
          throw;
        }
      }
      else
      {
        throw;
      }
    }
  }
  while (Again);
}