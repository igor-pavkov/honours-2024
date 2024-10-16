void __fastcall TSCPFileSystem::DeleteFile(const UnicodeString FileName,
  const TRemoteFile * File, int Params, TRmSessionAction & Action)
{
  DebugUsedParam(File);
  DebugUsedParam(Params);
  Action.Recursive();
  DebugAssert(FLAGCLEAR(Params, dfNoRecursive) || (File && File->IsSymLink));
  ExecCommand(fsDeleteFile, ARRAYOFCONST((DelimitStr(FileName))));
}