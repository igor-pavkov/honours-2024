TEST_F(RenameCollectionTest, RenameCollectionForApplyOpsRejectsRenameOpTimeIfWritesAreReplicated) {
    ASSERT_TRUE(_opCtx->writesAreReplicated());

    _createCollection(_opCtx.get(), _sourceNss);
    auto dbName = _sourceNss.db().toString();
    auto cmd = BSON("renameCollection" << _sourceNss.ns() << "to" << _targetNss.ns());
    auto renameOpTime = _opObserver->renameOpTime;
    ASSERT_EQUALS(ErrorCodes::BadValue,
                  renameCollectionForApplyOps(_opCtx.get(), dbName, {}, cmd, renameOpTime));
}