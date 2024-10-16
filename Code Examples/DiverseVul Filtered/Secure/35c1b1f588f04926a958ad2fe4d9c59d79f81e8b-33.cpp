TEST_F(RenameCollectionTest, RenameDifferentDatabaseStayTempFalseSourceNotTemporary) {
    _testRenameCollectionStayTemp(_opCtx.get(), _sourceNss, _targetNssDifferentDb, false, false);
}