void OpObserverMock::_logOp(OperationContext* opCtx,
                            const NamespaceString& nss,
                            const std::string& operationName) {
    if (repl::ReplicationCoordinator::get(opCtx)->isOplogDisabledFor(opCtx, nss)) {
        return;
    }
    oplogEntries.push_back(operationName);
}