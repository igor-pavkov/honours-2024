bool __fastcall TSCPFileSystem::IsCapable(int Capability) const
{
  DebugAssert(FTerminal);
  switch (Capability) {
    case fcUserGroupListing:
    case fcModeChanging:
    case fcModeChangingUpload:
    case fcPreservingTimestampUpload:
    case fcGroupChanging:
    case fcOwnerChanging:
    case fcAnyCommand:
    case fcShellAnyCommand:
    case fcHardLink:
    case fcSymbolicLink:
    case fcResolveSymlink:
    case fcRename:
    case fcRemoteMove:
    case fcRemoteCopy:
    case fcRemoveCtrlZUpload:
    case fcRemoveBOMUpload:
      return true;

    case fcTextMode:
      return FTerminal->SessionData->EOLType != FTerminal->Configuration->LocalEOLType;

    case fcNativeTextMode:
    case fcNewerOnlyUpload:
    case fcTimestampChanging:
    case fcLoadingAdditionalProperties:
    case fcCheckingSpaceAvailable:
    case fcIgnorePermErrors:
    case fcCalculatingChecksum:
    case fcSecondaryShell: // has fcShellAnyCommand
    case fcGroupOwnerChangingByID: // by name
    case fcMoveToQueue:
    case fcLocking:
    case fcPreservingTimestampDirs:
    case fcResumeSupport:
    case fsSkipTransfer:
    case fsParallelTransfers: // does not implement cpNoRecurse
      return false;

    case fcChangePassword:
      return FSecureShell->CanChangePassword();

    default:
      DebugFail();
      return false;
  }
}