TRemoteFile * __fastcall TSCPFileSystem::CreateRemoteFile(
  const UnicodeString & ListingStr, TRemoteFile * LinkedByFile)
{
  TRemoteFile * File = new TRemoteFile(LinkedByFile);
  try
  {
    File->Terminal = FTerminal;
    File->ListingStr = ListingStr;
    File->ShiftTimeInSeconds(TimeToSeconds(FTerminal->SessionData->TimeDifference));
    File->Complete();
  }
  catch(...)
  {
    delete File;
    throw;
  }

  return File;
}