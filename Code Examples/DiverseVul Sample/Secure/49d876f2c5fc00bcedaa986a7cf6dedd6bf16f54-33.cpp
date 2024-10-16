const TFileSystemInfo & __fastcall TSCPFileSystem::GetFileSystemInfo(bool Retrieve)
{
  if (FFileSystemInfo.AdditionalInfo.IsEmpty() && Retrieve)
  {
    UnicodeString UName;
    FTerminal->ExceptionOnFail = true;
    try
    {
      try
      {
        AnyCommand(L"uname -a", NULL);
        for (int Index = 0; Index < Output->Count; Index++)
        {
          if (Index > 0)
          {
            UName += L"; ";
          }
          UName += Output->Strings[Index];
        }
      }
      catch(...)
      {
        if (!FTerminal->Active)
        {
          throw;
        }
      }
    }
    __finally
    {
      FTerminal->ExceptionOnFail = false;
    }

    FFileSystemInfo.RemoteSystem = UName;
  }

  return FFileSystemInfo;
}