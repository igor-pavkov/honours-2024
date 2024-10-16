TEST_F(RenameCollectionTest, RenameDifferentDatabaseStayTempTrueSourceNotTemporary) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNssDifferentDb, true, false);
}