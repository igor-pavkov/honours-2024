TEST_F(RenameCollectionTest, RenameSameDatabaseStayTempTrue) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNss, true, true);
}