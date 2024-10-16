NamespaceString getNamespaceFromUUIDElement(OperationContext* opCtx, const BSONElement& ui) {
    if (ui.eoo())
        return {};
    auto uuid = uassertStatusOK(UUID::parse(ui));
    return getNamespaceFromUUID(opCtx, uuid);
}