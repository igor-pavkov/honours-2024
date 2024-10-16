TEST_F(RenameCollectionTest, RenameSameDatabaseStayTempTrueSourceNotTemporary) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNss, true, false);
}