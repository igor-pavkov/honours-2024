void _testRenameCollectionStayTemp(OperationContext* opCtx,
                                   const NamespaceString& sourceNss,
                                   const NamespaceString& targetNss,
                                   bool stayTemp,
                                   bool isSourceCollectionTemporary) {
    CollectionOptions collectionOptions;
    collectionOptions.temp = isSourceCollectionTemporary;
    _createCollection(opCtx, sourceNss, collectionOptions);

    RenameCollectionOptions options;
    options.stayTemp = stayTemp;
    ASSERT_OK(renameCollection(opCtx, sourceNss, targetNss, options));
    ASSERT_FALSE(_collectionExists(opCtx, sourceNss)) << "source collection " << sourceNss
                                                      << " still exists after successful rename";

    if (!isSourceCollectionTemporary) {
        ASSERT_FALSE(_isTempCollection(opCtx, targetNss))
            << "target collection " << targetNss
            << " cannot not be temporary after rename if source collection is not temporary.";
    } else if (stayTemp) {
        ASSERT_TRUE(_isTempCollection(opCtx, targetNss))
            << "target collection " << targetNss
            << " is no longer temporary after rename with stayTemp set to true.";
    } else {
        ASSERT_FALSE(_isTempCollection(opCtx, targetNss))
            << "target collection " << targetNss
            << " still temporary after rename with stayTemp set to false.";
    }
}