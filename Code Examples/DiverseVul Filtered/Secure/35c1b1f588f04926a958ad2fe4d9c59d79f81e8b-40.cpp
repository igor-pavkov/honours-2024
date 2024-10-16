TEST_F(RenameCollectionTest, RenameDifferentDatabaseStayTempTrue) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNssDifferentDb, true, true);
}