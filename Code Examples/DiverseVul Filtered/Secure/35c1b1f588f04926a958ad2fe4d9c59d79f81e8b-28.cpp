void OpObserverMock::onCreateCollection(OperationContext* opCtx,
                                        Collection* coll,
                                        const NamespaceString& collectionName,
                                        const CollectionOptions& options,
                                        const BSONObj& idIndex,
                                        const OplogSlot& createOpTime) {
    _logOp(opCtx, collectionName, "create");
    OpObserverNoop::onCreateCollection(opCtx, coll, collectionName, options, idIndex, createOpTime);
}