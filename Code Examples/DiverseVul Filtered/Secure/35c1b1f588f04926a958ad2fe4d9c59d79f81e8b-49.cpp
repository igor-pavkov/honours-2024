void _createIndexOnEmptyCollection(OperationContext* opCtx,
                                   const NamespaceString& nss,
                                   const std::string& indexName) {
    writeConflictRetry(opCtx, "_createIndexOnEmptyCollection", nss.ns(), [=] {
        AutoGetCollection autoColl(opCtx, nss, MODE_X);
        auto collection = autoColl.getCollection();
        ASSERT_TRUE(collection) << "Cannot create index on empty collection " << nss
                                << " because collection " << nss << " does not exist.";

        auto indexInfoObj = BSON(
            "v" << int(IndexDescriptor::kLatestIndexVersion) << "key" << BSON("a" << 1) << "name"
                << indexName
                << "ns"
                << nss.ns());

        auto indexCatalog = collection->getIndexCatalog();
        WriteUnitOfWork wuow(opCtx);
        ASSERT_OK(indexCatalog->createIndexOnEmptyCollection(opCtx, indexInfoObj).getStatus());
        wuow.commit();
    });

    ASSERT_TRUE(AutoGetCollectionForRead(opCtx, nss).getCollection());
}