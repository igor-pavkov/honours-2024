    Status CmdAuthenticate::_authenticateCR(const UserName& user, const BSONObj& cmdObj) {

        if (user == internalSecurity.user->getName() &&
            serverGlobalParams.clusterAuthMode.load() == 
            ServerGlobalParams::ClusterAuthMode_x509) {
            return Status(ErrorCodes::AuthenticationFailed,
                          "Mechanism x509 is required for internal cluster authentication");
        }

        if (_isCRAuthDisabled) {
            // SERVER-8461, MONGODB-CR must be enabled for authenticating the internal user, so that
            // cluster members may communicate with each other.
            if (user != internalSecurity.user->getName()) {
                return Status(ErrorCodes::BadValue, _nonceAuthenticationDisabledMessage);
            }
        }

        string key = cmdObj.getStringField("key");
        string received_nonce = cmdObj.getStringField("nonce");

        if( user.getUser().empty() || key.empty() || received_nonce.empty() ) {
            sleepmillis(10);
            return Status(ErrorCodes::ProtocolError,
                          "field missing/wrong type in received authenticate command");
        }

        stringstream digestBuilder;

        {
            ClientBasic *client = ClientBasic::getCurrent();
            boost::scoped_ptr<AuthenticationSession> session;
            client->swapAuthenticationSession(session);
            if (!session || session->getType() != AuthenticationSession::SESSION_TYPE_MONGO) {
                sleepmillis(30);
                return Status(ErrorCodes::ProtocolError, "No pending nonce");
            }
            else {
                nonce64 nonce = static_cast<MongoAuthenticationSession*>(session.get())->getNonce();
                digestBuilder << hex << nonce;
                if (digestBuilder.str() != received_nonce) {
                    sleepmillis(30);
                    return Status(ErrorCodes::AuthenticationFailed, "Received wrong nonce.");
                }
            }
        }

        User* userObj;
        Status status = getGlobalAuthorizationManager()->acquireUser(user, &userObj);
        if (!status.isOK()) {
            // Failure to find the privilege document indicates no-such-user, a fact that we do not
            // wish to reveal to the client.  So, we return AuthenticationFailed rather than passing
            // through the returned status.
            return Status(ErrorCodes::AuthenticationFailed, status.toString());
        }
        string pwd = userObj->getCredentials().password;
        getGlobalAuthorizationManager()->releaseUser(userObj);

        md5digest d;
        {
            digestBuilder << user.getUser() << pwd;
            string done = digestBuilder.str();

            md5_state_t st;
            md5_init(&st);
            md5_append(&st, (const md5_byte_t *) done.c_str(), done.size());
            md5_finish(&st, d);
        }

        string computed = digestToString( d );

        if ( key != computed ) {
            return Status(ErrorCodes::AuthenticationFailed, "key mismatch");
        }

        AuthorizationSession* authorizationSession =
            ClientBasic::getCurrent()->getAuthorizationSession();
        status = authorizationSession->addAndAuthorizeUser(user);
        if (!status.isOK()) {
            return status;
        }

        return Status::OK();
    }