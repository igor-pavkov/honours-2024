TEST_F(RenameCollectionTest, RenameCollectionAcrossDatabaseOplogEntriesWritesNotReplicated) {
    repl::UnreplicatedWritesBlock uwb(_opCtx.get());
    bool forApplyOps = false;
    _testRenameCollectionAcrossDatabaseOplogEntries(_opCtx.get(),
                                                    _sourceNss,
                                                    _targetNssDifferentDb,
                                                    &_opObserver->oplogEntries,
                                                    forApplyOps,
                                                    {});
}