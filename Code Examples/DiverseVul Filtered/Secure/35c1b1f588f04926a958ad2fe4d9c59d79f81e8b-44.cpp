void OpObserverMock::onCreateIndex(OperationContext* opCtx,
                                   const NamespaceString& nss,
                                   CollectionUUID uuid,
                                   BSONObj indexDoc,
                                   bool fromMigrate) {
    _logOp(opCtx, nss, "index");
    OpObserverNoop::onCreateIndex(opCtx, nss, uuid, indexDoc, fromMigrate);
}