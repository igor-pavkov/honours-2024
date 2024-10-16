UnicodeString __fastcall TSCPFileSystem::AbsolutePath(UnicodeString Path, bool /*Local*/)
{
  return ::AbsolutePath(CurrentDirectory, Path);
}