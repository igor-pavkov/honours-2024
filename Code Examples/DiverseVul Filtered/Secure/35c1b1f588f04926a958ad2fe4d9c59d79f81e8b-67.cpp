Status renameCollectionCommon(OperationContext* opCtx,
                              const NamespaceString& source,
                              const NamespaceString& target,
                              OptionalCollectionUUID targetUUID,
                              repl::OpTime renameOpTimeFromApplyOps,
                              const RenameCollectionOptions& options) {
    // A valid 'renameOpTimeFromApplyOps' is not allowed when writes are replicated.
    if (!renameOpTimeFromApplyOps.isNull() && opCtx->writesAreReplicated()) {
        return Status(
            ErrorCodes::BadValue,
            "renameCollection() cannot accept a rename optime when writes are replicated.");
    }

    DisableDocumentValidation validationDisabler(opCtx);

    boost::optional<Lock::GlobalWrite> globalWriteLock;
    boost::optional<Lock::DBLock> dbWriteLock;

    // If the rename is known not to be a cross-database rename, just a database lock suffices.
    auto lockState = opCtx->lockState();
    if (source.db() == target.db())
        dbWriteLock.emplace(opCtx, source.db(), MODE_X);
    else if (!lockState->isW())
        globalWriteLock.emplace(opCtx);

    // We stay in source context the whole time. This is mostly to set the CurOp namespace.
    boost::optional<OldClientContext> ctx;
    const bool shardVersionCheck = true;
    ctx.emplace(opCtx, source.ns(), shardVersionCheck);

    auto replCoord = repl::ReplicationCoordinator::get(opCtx);
    bool userInitiatedWritesAndNotPrimary =
        opCtx->writesAreReplicated() && !replCoord->canAcceptWritesFor(opCtx, source);

    if (userInitiatedWritesAndNotPrimary) {
        return Status(ErrorCodes::NotMaster,
                      str::stream() << "Not primary while renaming collection " << source << " to "
                                    << target);
    }

    auto databaseHolder = DatabaseHolder::get(opCtx);
    auto sourceDB = databaseHolder->getDb(opCtx, source.db());
    if (sourceDB) {
        auto& dss = DatabaseShardingState::get(sourceDB);
        auto dssLock = DatabaseShardingState::DSSLock::lock(opCtx, &dss);
        dss.checkDbVersion(opCtx, dssLock);
    }
    Collection* const sourceColl = sourceDB ? sourceDB->getCollection(opCtx, source) : nullptr;
    if (!sourceColl) {
        if (sourceDB && sourceDB->getViewCatalog()->lookup(opCtx, source.ns()))
            return Status(ErrorCodes::CommandNotSupportedOnView,
                          str::stream() << "cannot rename view: " << source);
        return Status(ErrorCodes::NamespaceNotFound, "source namespace does not exist");
    }

    // Make sure the source collection is not sharded.
    {
        auto* const css = CollectionShardingState::get(opCtx, source);
        const auto metadata = css->getCurrentMetadata();
        if (metadata->isSharded())
            return {ErrorCodes::IllegalOperation, "source namespace cannot be sharded"};
    }

    // Disallow renaming from a replicated to an unreplicated collection or vice versa.
    auto sourceIsUnreplicated = replCoord->isOplogDisabledFor(opCtx, source);
    auto targetIsUnreplicated = replCoord->isOplogDisabledFor(opCtx, target);
    if (sourceIsUnreplicated != targetIsUnreplicated) {
        return {ErrorCodes::IllegalOperation,
                "Cannot rename collections between a replicated and an unreplicated database"};
    }

    // Ensure that collection name does not exceed maximum length.
    // Index names do not limit the maximum allowable length of the target namespace under
    // FCV 4.2 and above.
    const auto checkIndexNamespace =
        serverGlobalParams.featureCompatibility.isVersionInitialized() &&
        serverGlobalParams.featureCompatibility.getVersion() !=
            ServerGlobalParams::FeatureCompatibility::Version::kFullyUpgradedTo42;
    std::string::size_type longestIndexNameLength =
        checkIndexNamespace ? sourceColl->getIndexCatalog()->getLongestIndexNameLength(opCtx) : 0;
    auto status = target.checkLengthForRename(longestIndexNameLength);
    if (!status.isOK()) {
        return status;
    }

    BackgroundOperation::assertNoBgOpInProgForNs(source.ns());

    auto targetDB = databaseHolder->openDb(opCtx, target.db());

    // Check if the target namespace exists and if dropTarget is true.
    // Return a non-OK status if target exists and dropTarget is not true or if the collection
    // is sharded.
    Collection* targetColl = targetDB->getCollection(opCtx, target);
    if (targetColl) {
        // If we already have the collection with the target UUID, we found our future selves,
        // so nothing left to do.
        if (targetUUID && targetUUID == targetColl->uuid()) {
            invariant(source == target);
            return Status::OK();
        }

        {
            auto* const css = CollectionShardingState::get(opCtx, target);
            const auto metadata = css->getCurrentMetadata();
            if (metadata->isSharded())
                return {ErrorCodes::IllegalOperation, "cannot rename to a sharded collection"};
        }

        if (!options.dropTarget) {
            return Status(ErrorCodes::NamespaceExists, "target namespace exists");
        }

        // If UUID doesn't point to the existing target, we should rename the target rather than
        // drop it.
        if (options.dropTargetUUID && options.dropTargetUUID != targetColl->uuid()) {
            auto dropTargetNssFromUUID = getNamespaceFromUUID(opCtx, options.dropTargetUUID.get());
            // We need to rename the targetColl to a temporary name.
            auto status = renameTargetCollectionToTmp(
                opCtx, source, targetUUID.get(), targetDB, target, targetColl->uuid().get());
            if (!status.isOK())
                return status;
            targetColl = nullptr;
        }
    } else if (targetDB->getViewCatalog()->lookup(opCtx, target.ns())) {
        return Status(ErrorCodes::NamespaceExists,
                      str::stream() << "a view already exists with that name: " << target);
    }

    // When reapplying oplog entries (such as in the case of initial sync) we need
    // to identify the collection to drop by UUID, as otherwise we might end up
    // dropping the wrong collection.
    if (!targetColl && options.dropTargetUUID) {
        invariant(options.dropTarget);
        auto dropTargetNssFromUUID = getNamespaceFromUUID(opCtx, options.dropTargetUUID.get());
        if (!dropTargetNssFromUUID.isEmpty() && !dropTargetNssFromUUID.isDropPendingNamespace()) {
            invariant(dropTargetNssFromUUID.db() == target.db());
            targetColl = targetDB->getCollection(opCtx, dropTargetNssFromUUID);
        }
    }

    auto opObserver = opCtx->getServiceContext()->getOpObserver();

    auto sourceUUID = sourceColl->uuid();
    // If we are renaming in the same database, just rename the namespace and we're done.
    if (sourceDB == targetDB) {
        return writeConflictRetry(opCtx, "renameCollection", target.ns(), [&] {
            WriteUnitOfWork wunit(opCtx);
            if (!targetColl) {
                // Target collection does not exist.
                auto stayTemp = options.stayTemp;
                {
                    // No logOp necessary because the entire renameCollection command is one logOp.
                    repl::UnreplicatedWritesBlock uwb(opCtx);
                    status = targetDB->renameCollection(opCtx, source.ns(), target.ns(), stayTemp);
                    if (!status.isOK()) {
                        return status;
                    }
                }
                // Rename is not resilient to interruption when the onRenameCollection OpObserver
                // takes an oplog collection lock.
                UninterruptibleLockGuard noInterrupt(opCtx->lockState());

                // We have to override the provided 'dropTarget' setting for idempotency reasons to
                // avoid unintentionally removing a collection on a secondary with the same name as
                // the target.
                opObserver->onRenameCollection(opCtx, source, target, sourceUUID, {}, 0U, stayTemp);
                wunit.commit();
                return Status::OK();
            }

            // Target collection exists - drop it.
            invariant(options.dropTarget);
            auto dropTargetUUID = targetColl->uuid();
            invariant(dropTargetUUID);

            // If this rename collection is replicated, check for long index names in the target
            // collection that may exceed the MMAPv1 namespace limit when the target collection
            // is renamed with a drop-pending namespace.
            auto isOplogDisabledForNamespace = replCoord->isOplogDisabledFor(opCtx, target);
            if (!isOplogDisabledForNamespace) {
                invariant(opCtx->writesAreReplicated());
                invariant(renameOpTimeFromApplyOps.isNull());

                // Compile a list of any indexes that would become too long following the
                // drop-pending rename. In the case that this collection drop gets rolled back, this
                // will incur a performance hit, since those indexes will have to be rebuilt from
                // scratch, but data integrity is maintained.
                std::vector<const IndexDescriptor*> indexesToDrop;
                auto indexIter = targetColl->getIndexCatalog()->getIndexIterator(opCtx, true);

                // Determine which index names are too long. Since we don't have the collection
                // rename optime at this time, use the maximum optime to check the index names.
                auto longDpns = target.makeDropPendingNamespace(repl::OpTime::max());
                while (indexIter->more()) {
                    auto index = indexIter->next()->descriptor();
                    auto status = longDpns.checkLengthForRename(index->indexName().size());
                    if (!status.isOK()) {
                        indexesToDrop.push_back(index);
                    }
                }

                // Drop the offending indexes.
                auto sourceUuidString = sourceUUID ? sourceUUID.get().toString() : "no UUID";
                auto dropTargetUuidString =
                    dropTargetUUID ? dropTargetUUID.get().toString() : "no UUID";
                for (auto&& index : indexesToDrop) {
                    log() << "renameCollection: renaming collection " << sourceUuidString
                          << " from " << source << " to " << target << " (" << dropTargetUuidString
                          << ") - target collection contains an index namespace '"
                          << index->indexNamespace()
                          << "' that would be too long after drop-pending rename. Dropping index "
                             "immediately.";
                    fassert(50941, targetColl->getIndexCatalog()->dropIndex(opCtx, index));
                    opObserver->onDropIndex(
                        opCtx, target, targetColl->uuid(), index->indexName(), index->infoObj());
                }
            }

            auto numRecords = targetColl->numRecords(opCtx);
            auto renameOpTime = opObserver->preRenameCollection(
                opCtx, source, target, sourceUUID, dropTargetUUID, numRecords, options.stayTemp);

            if (!renameOpTimeFromApplyOps.isNull()) {
                // 'renameOpTime' must be null because a valid 'renameOpTimeFromApplyOps' implies
                // replicated writes are not enabled.
                if (!renameOpTime.isNull()) {
                    severe() << "renameCollection: " << source << " to " << target
                             << " (with dropTarget=true) - unexpected renameCollection oplog entry"
                             << " written to the oplog with optime " << renameOpTime;
                    fassertFailed(40616);
                }
                renameOpTime = renameOpTimeFromApplyOps;
            }

            // No logOp necessary because the entire renameCollection command is one logOp.
            repl::UnreplicatedWritesBlock uwb(opCtx);

            status = targetDB->dropCollection(opCtx, targetColl->ns().ns(), renameOpTime);
            if (!status.isOK()) {
                return status;
            }

            status = targetDB->renameCollection(opCtx, source.ns(), target.ns(), options.stayTemp);
            if (!status.isOK()) {
                return status;
            }

            opObserver->postRenameCollection(
                opCtx, source, target, sourceUUID, dropTargetUUID, options.stayTemp);
            wunit.commit();
            return Status::OK();
        });
    }


    // If we get here, we are renaming across databases, so we must copy all the data and
    // indexes, then remove the source collection.

    // Create a temporary collection in the target database. It will be removed if we fail to copy
    // the collection, or on restart, so there is no need to replicate these writes.
    auto tmpNameResult =
        targetDB->makeUniqueCollectionNamespace(opCtx, "tmp%%%%%.renameCollection");
    if (!tmpNameResult.isOK()) {
        return tmpNameResult.getStatus().withContext(
            str::stream() << "Cannot generate temporary collection name to rename " << source
                          << " to "
                          << target);
    }
    const auto& tmpName = tmpNameResult.getValue();

    // Check if all the source collection's indexes can be recreated in the temporary collection.
    status = tmpName.checkLengthForRename(longestIndexNameLength);
    if (!status.isOK()) {
        return status;
    }

    Collection* tmpColl = nullptr;
    OptionalCollectionUUID newUUID;
    {
        auto collectionOptions = sourceColl->getCatalogEntry()->getCollectionOptions(opCtx);

        // Renaming across databases will result in a new UUID, as otherwise we'd require
        // two collections with the same uuid (temporarily).
        if (targetUUID)
            newUUID = targetUUID;
        else if (collectionOptions.uuid)
            newUUID = UUID::gen();

        collectionOptions.uuid = newUUID;

        writeConflictRetry(opCtx, "renameCollection", tmpName.ns(), [&] {
            WriteUnitOfWork wunit(opCtx);
            tmpColl = targetDB->createCollection(opCtx, tmpName.ns(), collectionOptions);
            wunit.commit();
        });
    }

    // Dismissed on success
    auto tmpCollectionDropper = makeGuard([&] {
        BSONObjBuilder unusedResult;
        Status status = Status::OK();
        try {
            status =
                dropCollection(opCtx,
                               tmpName,
                               unusedResult,
                               renameOpTimeFromApplyOps,
                               DropCollectionSystemCollectionMode::kAllowSystemCollectionDrops);
        } catch (...) {
            status = exceptionToStatus();
        }
        if (!status.isOK()) {
            // Ignoring failure case when dropping the temporary collection during cleanup because
            // the rename operation has already failed for another reason.
            log() << "Unable to drop temporary collection " << tmpName << " while renaming from "
                  << source << " to " << target << ": " << status;
        }
    });

    // Copy the index descriptions from the source collection, adjusting the ns field.
    {
        std::vector<BSONObj> indexesToCopy;
        std::unique_ptr<IndexCatalog::IndexIterator> sourceIndIt =
            sourceColl->getIndexCatalog()->getIndexIterator(opCtx, true);
        while (sourceIndIt->more()) {
            auto descriptor = sourceIndIt->next()->descriptor();
            if (descriptor->isIdIndex()) {
                continue;
            }

            const BSONObj currIndex = descriptor->infoObj();

            // Process the source index, adding fields in the same order as they were originally.
            BSONObjBuilder newIndex;
            for (auto&& elem : currIndex) {
                if (elem.fieldNameStringData() == "ns") {
                    newIndex.append("ns", tmpName.ns());
                } else {
                    newIndex.append(elem);
                }
            }
            indexesToCopy.push_back(newIndex.obj());
        }

        // Create indexes using the namespace-adjusted index specs on the empty temporary collection
        // that was just created. Since each index build is possibly replicated to downstream nodes,
        // each createIndex oplog entry must have a distinct timestamp to support correct rollback
        // operation. This is achieved by writing the createIndexes oplog entry *before* creating
        // the index. Using IndexCatalog::createIndexOnEmptyCollection() for the index creation
        // allows us to add and commit the index within a single WriteUnitOfWork and avoids the
        // possibility of seeing the index in an unfinished state. For more information on assigning
        // timestamps to multiple index builds, please see SERVER-35780 and SERVER-35070.
        status = writeConflictRetry(opCtx, "renameCollection", tmpName.ns(), [&] {
            WriteUnitOfWork wunit(opCtx);
            auto tmpIndexCatalog = tmpColl->getIndexCatalog();
            for (const auto& indexToCopy : indexesToCopy) {
                opObserver->onCreateIndex(opCtx,
                                          tmpName,
                                          *(tmpColl->uuid()),
                                          indexToCopy,
                                          false  // fromMigrate
                                          );
                auto indexResult =
                    tmpIndexCatalog->createIndexOnEmptyCollection(opCtx, indexToCopy);
                if (!indexResult.isOK()) {
                    return indexResult.getStatus();
                }
            };
            wunit.commit();
            return Status::OK();
        });
        if (!status.isOK()) {
            return status;
        }
    }

    {
        // Copy over all the data from source collection to temporary collection.
        // We do not need global write exclusive access after obtaining the collection locks on the
        // source and temporary collections. After copying the documents, each remaining stage of
        // the cross-database rename will be responsible for its own lock management.
        // Therefore, unless the caller has already acquired the global write lock prior to invoking
        // this function, we relinquish global write access to the database after acquiring the
        // collection locks.
        // Collection locks must be obtained while holding the global lock to avoid any possibility
        // of a deadlock.
        AutoGetCollectionForRead autoSourceColl(opCtx, source);
        AutoGetCollection autoTmpColl(opCtx, tmpName, MODE_IX);
        ctx.reset();

        if (opCtx->getServiceContext()->getStorageEngine()->supportsDBLocking()) {
            if (globalWriteLock) {
                const ResourceId globalLockResourceId(RESOURCE_GLOBAL,
                                                      ResourceId::SINGLETON_GLOBAL);
                lockState->downgrade(globalLockResourceId, MODE_IX);
                invariant(!lockState->isW());
            } else {
                invariant(lockState->isW());
            }
        }

        auto cursor = sourceColl->getCursor(opCtx);
        auto record = cursor->next();
        while (record) {
            opCtx->checkForInterrupt();
            // Cursor is left one past the end of the batch inside writeConflictRetry.
            auto beginBatchId = record->id;
            status = writeConflictRetry(opCtx, "renameCollection", tmpName.ns(), [&] {
                WriteUnitOfWork wunit(opCtx);
                // Need to reset cursor if it gets a WCE midway through.
                if (!record || (beginBatchId != record->id)) {
                    record = cursor->seekExact(beginBatchId);
                }
                for (int i = 0; record && (i < internalInsertMaxBatchSize.load()); i++) {
                    const InsertStatement stmt(record->data.releaseToBson());
                    OpDebug* const opDebug = nullptr;
                    auto status = tmpColl->insertDocument(opCtx, stmt, opDebug, true);
                    if (!status.isOK()) {
                        return status;
                    }
                    record = cursor->next();
                }
                cursor->save();
                // When this exits via success or WCE, we need to restore the cursor.
                ON_BLOCK_EXIT([ opCtx, ns = tmpName.ns(), &cursor ]() {
                    writeConflictRetry(
                        opCtx, "retryRestoreCursor", ns, [&cursor] { cursor->restore(); });
                });
                // Used to make sure that a WCE can be handled by this logic without data loss.
                if (MONGO_FAIL_POINT(writeConfilctInRenameCollCopyToTmp)) {
                    throw WriteConflictException();
                }
                wunit.commit();
                return Status::OK();
            });
            if (!status.isOK()) {
                return status;
            }
        }
    }
    globalWriteLock.reset();

    // Getting here means we successfully built the target copy. We now do the final
    // in-place rename and remove the source collection.
    invariant(tmpName.db() == target.db());
    status = renameCollectionCommon(
        opCtx, tmpName, target, targetUUID, renameOpTimeFromApplyOps, options);
    if (!status.isOK()) {
        return status;
    }
    tmpCollectionDropper.dismiss();

    BSONObjBuilder unusedResult;
    return dropCollection(opCtx,
                          source,
                          unusedResult,
                          renameOpTimeFromApplyOps,
                          DropCollectionSystemCollectionMode::kAllowSystemCollectionDrops);
}