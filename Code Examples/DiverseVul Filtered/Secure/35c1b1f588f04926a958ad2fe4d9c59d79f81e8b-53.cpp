CollectionUUID _createCollectionWithUUID(OperationContext* opCtx, const NamespaceString& nss) {
    const auto options = _makeCollectionOptionsWithUuid();
    _createCollection(opCtx, nss, options);
    return options.uuid.get();
}