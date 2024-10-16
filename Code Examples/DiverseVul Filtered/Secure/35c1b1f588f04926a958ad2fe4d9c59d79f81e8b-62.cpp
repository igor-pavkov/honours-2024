TEST_F(RenameCollectionTest,
       RenameCollectionForApplyOpsAcrossDatabaseOplogEntriesWritesNotReplicated) {
    repl::UnreplicatedWritesBlock uwb(_opCtx.get());
    bool forApplyOps = true;
    _testRenameCollectionAcrossDatabaseOplogEntries(_opCtx.get(),
                                                    _sourceNss,
                                                    _targetNssDifferentDb,
                                                    &_opObserver->oplogEntries,
                                                    forApplyOps,
                                                    {});
}