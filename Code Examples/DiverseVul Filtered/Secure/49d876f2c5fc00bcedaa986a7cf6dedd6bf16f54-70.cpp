void __fastcall TSCPFileSystem::CopyFile(const UnicodeString FileName, const TRemoteFile * /*File*/,
  const UnicodeString NewName)
{
  UnicodeString DelimitedFileName = DelimitStr(FileName);
  UnicodeString DelimitedNewName = DelimitStr(NewName);
  const UnicodeString AdditionalSwitches = L"-T";
  try
  {
    ExecCommand(fsCopyFile, ARRAYOFCONST((AdditionalSwitches, DelimitedFileName, DelimitedNewName)));
  }
  catch (Exception & E)
  {
    if (FTerminal->Active)
    {
      // The -T is GNU switch and may not be available on all platforms.
      // https://lists.gnu.org/archive/html/bug-coreutils/2004-07/msg00000.html
      FTerminal->LogEvent(FORMAT(L"Attempt with %s failed, trying without", (AdditionalSwitches)));
      ExecCommand(fsCopyFile, ARRAYOFCONST((L"", DelimitedFileName, DelimitedNewName)));
    }
    else
    {
      throw;
    }
  }
}