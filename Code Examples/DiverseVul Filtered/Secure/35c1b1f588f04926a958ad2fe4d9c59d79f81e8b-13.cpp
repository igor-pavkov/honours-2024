repl::OpTime OpObserverMock::onDropCollection(OperationContext* opCtx,
                                              const NamespaceString& collectionName,
                                              OptionalCollectionUUID uuid,
                                              std::uint64_t numRecords,
                                              const CollectionDropType dropType) {
    _logOp(opCtx, collectionName, "drop");
    // If the oplog is not disabled for this namespace, then we need to reserve an op time for the
    // drop.
    if (!repl::ReplicationCoordinator::get(opCtx)->isOplogDisabledFor(opCtx, collectionName)) {
        OpObserver::Times::get(opCtx).reservedOpTimes.push_back(dropOpTime);
    }
    auto noopOptime =
        OpObserverNoop::onDropCollection(opCtx, collectionName, uuid, numRecords, dropType);
    invariant(noopOptime.isNull());
    return {};
}