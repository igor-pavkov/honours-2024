void _createCollection(OperationContext* opCtx,
                       const NamespaceString& nss,
                       const CollectionOptions options = {}) {
    writeConflictRetry(opCtx, "_createCollection", nss.ns(), [=] {
        AutoGetOrCreateDb autoDb(opCtx, nss.db(), MODE_X);
        auto db = autoDb.getDb();
        ASSERT_TRUE(db) << "Cannot create collection " << nss << " because database " << nss.db()
                        << " does not exist.";

        WriteUnitOfWork wuow(opCtx);
        ASSERT_TRUE(db->createCollection(opCtx, nss.ns(), options))
            << "Failed to create collection " << nss << " due to unknown error.";
        wuow.commit();
    });

    ASSERT_TRUE(AutoGetCollectionForRead(opCtx, nss).getCollection());
}