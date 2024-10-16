void OpObserverMock::onInserts(OperationContext* opCtx,
                               const NamespaceString& nss,
                               OptionalCollectionUUID uuid,
                               std::vector<InsertStatement>::const_iterator begin,
                               std::vector<InsertStatement>::const_iterator end,
                               bool fromMigrate) {
    if (onInsertsThrows) {
        uasserted(ErrorCodes::OperationFailed, "insert failed");
    }

    // Check global lock state.
    auto lockState = opCtx->lockState();
    ASSERT_TRUE(lockState->isWriteLocked());
    onInsertsIsGlobalWriteLockExclusive = lockState->isW();

    _logOp(opCtx, nss, "inserts");
    OpObserverNoop::onInserts(opCtx, nss, uuid, begin, end, fromMigrate);
}