TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsDropTargetByUUIDTargetExists) {
    const auto& collA = NamespaceString("test.A");
    const auto& collB = NamespaceString("test.B");
    const auto& collC = NamespaceString("test.C");
    auto dbName = collA.db().toString();
    auto collAUUID = _createCollectionWithUUID(_opCtx.get(), collA);
    auto collBUUID = _createCollectionWithUUID(_opCtx.get(), collB);
    auto collCUUID = _createCollectionWithUUID(_opCtx.get(), collC);
    auto uuidDoc = BSON("ui" << collAUUID);
    // Rename A to B, drop C, where B is an existing collection
    // B should be kept but with a temporary name
    auto cmd =
        BSON("renameCollection" << collA.ns() << "to" << collB.ns() << "dropTarget" << collCUUID);
    ASSERT_OK(renameCollectionForApplyOps(_opCtx.get(), dbName, uuidDoc["ui"], cmd, {}));
    // A and C should be dropped
    ASSERT_FALSE(_collectionExists(_opCtx.get(), collA));
    ASSERT_FALSE(_collectionExists(_opCtx.get(), collC));
    // B (originally A) should exist
    ASSERT_TRUE(_collectionExists(_opCtx.get(), collB));
    // The original B should exist too, but with a temporary name
    const auto& tmpB = UUIDCatalog::get(_opCtx.get()).lookupNSSByUUID(collBUUID);
    ASSERT_FALSE(tmpB.isEmpty());
    ASSERT_TRUE(tmpB.coll().startsWith("tmp"));
    ASSERT_TRUE(tmpB != collB);
}