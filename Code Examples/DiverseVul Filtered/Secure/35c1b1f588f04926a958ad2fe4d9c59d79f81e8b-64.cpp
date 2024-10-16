Status renameTargetCollectionToTmp(OperationContext* opCtx,
                                   const NamespaceString& sourceNs,
                                   const UUID& sourceUUID,
                                   Database* const targetDB,
                                   const NamespaceString& targetNs,
                                   const UUID& targetUUID) {
    repl::UnreplicatedWritesBlock uwb(opCtx);

    auto tmpNameResult = targetDB->makeUniqueCollectionNamespace(opCtx, "tmp%%%%%.rename");
    if (!tmpNameResult.isOK()) {
        return tmpNameResult.getStatus().withContext(
            str::stream() << "Cannot generate a temporary collection name for the target "
                          << targetNs
                          << " ("
                          << targetUUID
                          << ") so that the source"
                          << sourceNs
                          << " ("
                          << sourceUUID
                          << ") could be renamed to "
                          << targetNs);
    }
    const auto& tmpName = tmpNameResult.getValue();
    const bool stayTemp = true;
    return writeConflictRetry(opCtx, "renameCollection", targetNs.ns(), [&] {
        WriteUnitOfWork wunit(opCtx);
        auto status = targetDB->renameCollection(opCtx, targetNs.ns(), tmpName.ns(), stayTemp);
        if (!status.isOK())
            return status;

        wunit.commit();

        log() << "Successfully renamed the target " << targetNs << " (" << targetUUID << ") to "
              << tmpName << " so that the source " << sourceNs << " (" << sourceUUID
              << ") could be renamed to " << targetNs;

        return Status::OK();
    });
}