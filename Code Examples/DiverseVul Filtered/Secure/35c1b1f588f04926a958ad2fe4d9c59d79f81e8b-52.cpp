NamespaceString _getCollectionNssFromUUID(OperationContext* opCtx, const UUID& uuid) {
    Collection* source = UUIDCatalog::get(opCtx).lookupCollectionByUUID(uuid);
    return source ? source->ns() : NamespaceString();
}