    bool CmdAuthenticate::run(const string& dbname,
                              BSONObj& cmdObj,
                              int,
                              string& errmsg,
                              BSONObjBuilder& result,
                              bool fromRepl) {

        mutablebson::Document cmdToLog(cmdObj, mutablebson::Document::kInPlaceDisabled);
        redactForLogging(&cmdToLog);
        log() << " authenticate db: " << dbname << " " << cmdToLog << endl;

        UserName user(cmdObj.getStringField("user"), dbname);
        if (Command::testCommandsEnabled &&
                user.getDB() == "admin" &&
                user.getUser() == internalSecurity.user->getName().getUser()) {
            // Allows authenticating as the internal user against the admin database.  This is to
            // support the auth passthrough test framework on mongos (since you can't use the local
            // database on a mongos, so you can't auth as the internal user without this).
            user = internalSecurity.user->getName();
        }

        std::string mechanism = cmdObj.getStringField("mechanism");
        if (mechanism.empty()) {
            mechanism = "MONGODB-CR";
        }
        Status status = _authenticate(mechanism, user, cmdObj);
        audit::logAuthentication(ClientBasic::getCurrent(),
                                 mechanism,
                                 user,
                                 status.code());
        if (!status.isOK()) {
            log() << "Failed to authenticate " << user << " with mechanism " << mechanism << ": " <<
                status;
            if (status.code() == ErrorCodes::AuthenticationFailed) {
                // Statuses with code AuthenticationFailed may contain messages we do not wish to
                // reveal to the user, so we return a status with the message "auth failed".
                appendCommandStatus(result,
                                    Status(ErrorCodes::AuthenticationFailed, "auth failed"));
            }
            else {
                appendCommandStatus(result, status);
            }
            return false;
        }
        result.append("dbname", user.getDB());
        result.append("user", user.getUser());
        return true;
    }