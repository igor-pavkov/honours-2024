TEST_F(RenameCollectionTest, RenameSameDatabaseStayTempFalseSourceNotTemporary) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNss, false, false);
}