repl::OpTime OpObserverMock::preRenameCollection(OperationContext* opCtx,
                                                 const NamespaceString& fromCollection,
                                                 const NamespaceString& toCollection,
                                                 OptionalCollectionUUID uuid,
                                                 OptionalCollectionUUID dropTargetUUID,
                                                 std::uint64_t numRecords,
                                                 bool stayTemp) {
    _logOp(opCtx, fromCollection, "rename");
    OpObserver::Times::get(opCtx).reservedOpTimes.push_back(renameOpTime);
    OpObserverNoop::preRenameCollection(
        opCtx, fromCollection, toCollection, uuid, dropTargetUUID, numRecords, stayTemp);
    return {};
}