TEST_F(RenameCollectionTest, RenameCollectionReturnsNotMasterIfNotPrimary) {
    _createCollection(_opCtx.get(), _sourceNss);
    ASSERT_OK(_replCoord->setFollowerMode(repl::MemberState::RS_SECONDARY));
    ASSERT_TRUE(_opCtx->writesAreReplicated());
    ASSERT_FALSE(_replCoord->canAcceptWritesForDatabase(_opCtx.get(), _sourceNss.db()));
    ASSERT_EQUALS(ErrorCodes::NotMaster,
                  renameCollection(_opCtx.get(), _sourceNss, _targetNss, {}));
}