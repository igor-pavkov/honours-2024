void __fastcall TSCPFileSystem::ClearAlias(UnicodeString Alias)
{
  if (!Alias.IsEmpty())
  {
    // this command usually fails, because there will never be
    // aliases on all commands -> see last False parameter
    ExecCommand(fsUnalias, ARRAYOFCONST((Alias)), false);
  }
}