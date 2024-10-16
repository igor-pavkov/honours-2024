unsigned int __fastcall TSCPFileSystem::ConfirmOverwrite(
  const UnicodeString & SourceFullFileName, const UnicodeString & TargetFileName, TOperationSide Side,
  const TOverwriteFileParams * FileParams, const TCopyParamType * CopyParam,
  int Params, TFileOperationProgressType * OperationProgress)
{
  TSuspendFileOperationProgress Suspend(OperationProgress);

  TQueryButtonAlias Aliases[3];
  Aliases[0] = TQueryButtonAlias::CreateAllAsYesToNewerGrouppedWithYes();
  Aliases[1] = TQueryButtonAlias::CreateYesToAllGrouppedWithYes();
  Aliases[2] = TQueryButtonAlias::CreateNoToAllGrouppedWithNo();
  TQueryParams QueryParams(qpNeverAskAgainCheck);
  QueryParams.Aliases = Aliases;
  QueryParams.AliasesCount = LENOF(Aliases);
  unsigned int Answer =
    FTerminal->ConfirmFileOverwrite(
      SourceFullFileName, TargetFileName, FileParams,
      qaYes | qaNo | qaCancel | qaYesToAll | qaNoToAll | qaAll,
      &QueryParams, Side, CopyParam, Params, OperationProgress);
  return Answer;
}