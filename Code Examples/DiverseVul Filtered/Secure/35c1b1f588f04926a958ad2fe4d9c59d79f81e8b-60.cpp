Status renameCollection(OperationContext* opCtx,
                        const NamespaceString& source,
                        const NamespaceString& target,
                        const RenameCollectionOptions& options) {
    if (source.isDropPendingNamespace()) {
        return Status(ErrorCodes::NamespaceNotFound,
                      str::stream() << "renameCollection() cannot accept a source "
                                       "collection that is in a drop-pending state: "
                                    << source.toString());
    }

    const std::string dropTargetMsg =
        options.dropTarget ? " and drop " + target.toString() + "." : ".";
    log() << "renameCollectionForCommand: rename " << source << " to " << target << dropTargetMsg;

    OptionalCollectionUUID noTargetUUID;
    return renameCollectionCommon(opCtx, source, target, noTargetUUID, {}, options);
}