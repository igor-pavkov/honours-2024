Collection* _getCollection_inlock(OperationContext* opCtx, const NamespaceString& nss) {
    invariant(opCtx->lockState()->isCollectionLockedForMode(nss.ns(), MODE_IS));
    auto databaseHolder = DatabaseHolder::get(opCtx);
    auto* db = databaseHolder->getDb(opCtx, nss.db());
    if (!db) {
        return nullptr;
    }
    return db->getCollection(opCtx, nss.ns());
}