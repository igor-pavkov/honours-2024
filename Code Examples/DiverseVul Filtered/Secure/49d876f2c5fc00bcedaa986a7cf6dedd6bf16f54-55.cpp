void __fastcall TSCPFileSystem::ReadCurrentDirectory()
{
  if (FCachedDirectoryChange.IsEmpty())
  {
    ExecCommand(fsCurrentDirectory);
    FCurrentDirectory = UnixExcludeTrailingBackslash(FOutput->Strings[0]);
  }
  else
  {
    FCurrentDirectory = FCachedDirectoryChange;
  }
}