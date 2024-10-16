TEST_F(RenameCollectionTest, RenameSameDatabaseStayTempFalse) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNss, false, true);
}