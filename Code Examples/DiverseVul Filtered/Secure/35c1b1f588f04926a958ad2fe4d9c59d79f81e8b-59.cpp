ServiceContext::UniqueOperationContext RenameCollectionTest::makeOpCtx() {
    return cc().makeOperationContext();
}