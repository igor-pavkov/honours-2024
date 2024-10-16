void __fastcall TSCPFileSystem::Sink(
  const UnicodeString & /*FileName*/, const TRemoteFile * /*File*/,
  const UnicodeString & /*TargetDir*/, UnicodeString & /*DestFileName*/, int /*Attrs*/,
  const TCopyParamType * /*CopyParam*/, int /*Params*/, TFileOperationProgressType * /*OperationProgress*/,
  unsigned int /*Flags*/, TDownloadSessionAction & /*Action*/)
{
  DebugFail();
}