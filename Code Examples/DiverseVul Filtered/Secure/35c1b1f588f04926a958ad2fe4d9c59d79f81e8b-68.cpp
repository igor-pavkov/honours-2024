void _insertDocument(OperationContext* opCtx, const NamespaceString& nss, const BSONObj& doc) {
    writeConflictRetry(opCtx, "_insertDocument", nss.ns(), [=] {
        AutoGetCollection autoColl(opCtx, nss, MODE_X);
        auto collection = autoColl.getCollection();
        ASSERT_TRUE(collection) << "Cannot insert document " << doc << " into collection " << nss
                                << " because collection " << nss << " does not exist.";

        WriteUnitOfWork wuow(opCtx);
        OpDebug* const opDebug = nullptr;
        ASSERT_OK(collection->insertDocument(opCtx, InsertStatement(doc), opDebug));
        wuow.commit();
    });
}