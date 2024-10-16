void RenameCollectionTest::setUp() {
    // Set up mongod.
    ServiceContextMongoDTest::setUp();

    auto service = getServiceContext();
    _opCtx = cc().makeOperationContext();

    repl::StorageInterface::set(service, stdx::make_unique<repl::StorageInterfaceMock>());
    repl::DropPendingCollectionReaper::set(
        service,
        stdx::make_unique<repl::DropPendingCollectionReaper>(repl::StorageInterface::get(service)));

    // Set up ReplicationCoordinator and create oplog.
    auto replCoord = stdx::make_unique<repl::ReplicationCoordinatorMock>(service);
    _replCoord = replCoord.get();
    repl::ReplicationCoordinator::set(service, std::move(replCoord));
    repl::setOplogCollectionName(service);
    repl::createOplog(_opCtx.get());

    // Ensure that we are primary.
    ASSERT_OK(_replCoord->setFollowerMode(repl::MemberState::RS_PRIMARY));

    // Use OpObserverMock to track notifications for collection and database drops.
    auto opObserver = stdx::make_unique<OpObserverRegistry>();
    auto mockObserver = stdx::make_unique<OpObserverMock>();
    _opObserver = mockObserver.get();
    opObserver->addObserver(std::move(mockObserver));
    opObserver->addObserver(stdx::make_unique<UUIDCatalogObserver>());
    service->setOpObserver(std::move(opObserver));

    _sourceNss = NamespaceString("test.foo");
    _targetNss = NamespaceString("test.bar");
    _targetNssDifferentDb = NamespaceString("test2.bar");
}