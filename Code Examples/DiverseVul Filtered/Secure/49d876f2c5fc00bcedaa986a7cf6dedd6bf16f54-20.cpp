void __fastcall TSCPFileSystem::Source(
  TLocalFileHandle & /*Handle*/, const UnicodeString & /*TargetDir*/, UnicodeString & /*DestFileName*/,
  const TCopyParamType * /*CopyParam*/, int /*Params*/,
  TFileOperationProgressType * /*OperationProgress*/, unsigned int /*Flags*/,
  TUploadSessionAction & /*Action*/, bool & /*ChildError*/)
{
  DebugFail();
}