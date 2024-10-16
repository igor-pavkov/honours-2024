Status OplogApplicationChecks::checkOperationAuthorization(OperationContext* opCtx,
                                                           const std::string& dbname,
                                                           const BSONObj& oplogEntry,
                                                           AuthorizationSession* authSession,
                                                           bool alwaysUpsert) {
    BSONElement opTypeElem = oplogEntry["op"];
    checkBSONType(BSONType::String, opTypeElem);
    const StringData opType = opTypeElem.checkAndGetStringData();

    if (opType == "n"_sd) {
        // oplog notes require cluster permissions, and may not have a ns
        if (!authSession->isAuthorizedForActionsOnResource(ResourcePattern::forClusterResource(),
                                                           ActionType::appendOplogNote)) {
            return Status(ErrorCodes::Unauthorized, "Unauthorized");
        }
        return Status::OK();
    }

    BSONElement nsElem = oplogEntry["ns"];
    checkBSONType(BSONType::String, nsElem);
    NamespaceString ns(oplogEntry["ns"].checkAndGetStringData());

    if (oplogEntry.hasField("ui"_sd)) {
        // ns by UUID overrides the ns specified if they are different.
        auto& catalog = CollectionCatalog::get(opCtx);
        boost::optional<NamespaceString> uuidCollNS =
            catalog.lookupNSSByUUID(getUUIDFromOplogEntry(oplogEntry));
        if (uuidCollNS && *uuidCollNS != ns)
            ns = *uuidCollNS;
    }

    BSONElement oElem = oplogEntry["o"];
    checkBSONType(BSONType::Object, oElem);
    BSONObj o = oElem.Obj();

    if (opType == "c"_sd) {
        StringData commandName = o.firstElement().fieldNameStringData();
        Command* commandInOplogEntry = CommandHelpers::findCommand(commandName);
        if (!commandInOplogEntry) {
            return Status(ErrorCodes::FailedToParse, "Unrecognized command in op");
        }

        std::string dbNameForAuthCheck = ns.db().toString();
        if (commandName == "renameCollection") {
            // renameCollection commands must be run on the 'admin' database. Its arguments are
            // fully qualified namespaces. Catalog internals don't know the op produced by running
            // renameCollection was originally run on 'admin', so we must restore this.
            dbNameForAuthCheck = "admin";
        }

        // TODO reuse the parse result for when we run() later. Note that when running,
        // we must use a potentially different dbname.
        return [&] {
            try {
                auto request = OpMsgRequest::fromDBAndBody(dbNameForAuthCheck, o);
                commandInOplogEntry->parse(opCtx, request)->checkAuthorization(opCtx, request);
                return Status::OK();
            } catch (const DBException& e) {
                return e.toStatus();
            }
        }();
    }

    if (opType == "i"_sd) {
        return authSession->checkAuthForInsert(opCtx, ns);
    } else if (opType == "u"_sd) {
        BSONElement o2Elem = oplogEntry["o2"];
        checkBSONType(BSONType::Object, o2Elem);
        BSONObj o2 = o2Elem.Obj();

        BSONElement bElem = oplogEntry["b"];
        if (!bElem.eoo()) {
            checkBSONType(BSONType::Bool, bElem);
        }
        bool b = bElem.trueValue();

        const bool upsert = b || alwaysUpsert;

        return authSession->checkAuthForUpdate(opCtx, ns, o, o2, upsert);
    } else if (opType == "d"_sd) {

        return authSession->checkAuthForDelete(opCtx, ns, o);
    } else if (opType == "db"_sd) {
        // It seems that 'db' isn't used anymore. Require all actions to prevent casual use.
        ActionSet allActions;
        allActions.addAllActions();
        if (!authSession->isAuthorizedForActionsOnResource(ResourcePattern::forAnyResource(),
                                                           allActions)) {
            return Status(ErrorCodes::Unauthorized, "Unauthorized");
        }
        return Status::OK();
    }

    return Status(ErrorCodes::FailedToParse, "Unrecognized opType");
}