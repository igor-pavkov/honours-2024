void OpObserverMock::postRenameCollection(OperationContext* opCtx,
                                          const NamespaceString& fromCollection,
                                          const NamespaceString& toCollection,
                                          OptionalCollectionUUID uuid,
                                          OptionalCollectionUUID dropTargetUUID,
                                          bool stayTemp) {
    OpObserverNoop::postRenameCollection(
        opCtx, fromCollection, toCollection, uuid, dropTargetUUID, stayTemp);
    onRenameCollectionCalled = true;
    onRenameCollectionDropTarget = dropTargetUUID;
}