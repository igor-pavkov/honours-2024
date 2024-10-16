TEST_F(RenameCollectionTest, RenameDifferentDatabaseStayTempFalse) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNssDifferentDb, false, true);
}