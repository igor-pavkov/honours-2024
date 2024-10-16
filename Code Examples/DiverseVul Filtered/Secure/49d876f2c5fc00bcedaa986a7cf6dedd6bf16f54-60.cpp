void __fastcall TSCPFileSystem::CustomReadFile(const UnicodeString FileName,
  TRemoteFile *& File, TRemoteFile * ALinkedByFile)
{
  File = NULL;
  int Params = ecDefault |
    FLAGMASK(FTerminal->SessionData->IgnoreLsWarnings, ecIgnoreWarnings);
  // the auto-detection of --full-time support is not implemented for fsListFile,
  // so we use it only if we already know that it is supported (asOn).
  const wchar_t * Options = (FLsFullTime == asOn) ? FullTimeOption : L"";
  ExecCommand(fsListFile,
    ARRAYOFCONST((FTerminal->SessionData->ListingCommand, Options, DelimitStr(FileName))),
    Params);
  if (FOutput->Count)
  {
    int LineIndex = 0;
    if (IsTotalListingLine(FOutput->Strings[LineIndex]) && FOutput->Count > 1)
    {
      LineIndex++;
    }

    File = CreateRemoteFile(FOutput->Strings[LineIndex], ALinkedByFile);
  }
}