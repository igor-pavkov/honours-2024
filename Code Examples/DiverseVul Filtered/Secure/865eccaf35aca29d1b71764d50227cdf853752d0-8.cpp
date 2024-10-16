MONGO_INITIALIZER(AuthorizationBuiltinRoles)(InitializerContext* context) {
    // Read role
    readRoleActions
        << ActionType::changeStream
        << ActionType::collStats
        << ActionType::dbHash
        << ActionType::dbStats
        << ActionType::find
        << ActionType::killCursors
        << ActionType::listCollections
        << ActionType::listIndexes
        << ActionType::planCacheRead;

    // Read-write role
    readWriteRoleActions += readRoleActions;
    readWriteRoleActions
        << ActionType::convertToCapped  // db admin gets this also
        << ActionType::createCollection  // db admin gets this also
        << ActionType::dropCollection
        << ActionType::dropIndex
        << ActionType::emptycapped
        << ActionType::createIndex
        << ActionType::insert
        << ActionType::remove
        << ActionType::renameCollectionSameDB  // db admin gets this also
        << ActionType::update;

    // User admin role
    userAdminRoleActions
        << ActionType::changeCustomData
        << ActionType::changePassword
        << ActionType::createUser
        << ActionType::createRole
        << ActionType::dropUser
        << ActionType::dropRole
        << ActionType::grantRole
        << ActionType::revokeRole
        << ActionType::setAuthenticationRestriction
        << ActionType::viewUser
        << ActionType::viewRole;


    // DB admin role
    dbAdminRoleActions
        << ActionType::bypassDocumentValidation
        << ActionType::collMod
        << ActionType::collStats  // clusterMonitor gets this also
        << ActionType::compact
        << ActionType::convertToCapped  // read_write gets this also
        << ActionType::createCollection // read_write gets this also
        << ActionType::dbStats  // clusterMonitor gets this also
        << ActionType::dropCollection
        << ActionType::dropDatabase  // clusterAdmin gets this also TODO(spencer): should
                                     // readWriteAnyDatabase?
        << ActionType::dropIndex
        << ActionType::createIndex
        << ActionType::enableProfiler
        << ActionType::listCollections
        << ActionType::listIndexes
        << ActionType::planCacheIndexFilter
        << ActionType::planCacheRead
        << ActionType::planCacheWrite
        << ActionType::reIndex
        << ActionType::renameCollectionSameDB  // read_write gets this also
        << ActionType::storageDetails
        << ActionType::validate;

    // clusterMonitor role actions that target the cluster resource
    clusterMonitorRoleClusterActions
        << ActionType::checkFreeMonitoringStatus
        << ActionType::connPoolStats
        << ActionType::getCmdLineOpts
        << ActionType::getLog
        << ActionType::getParameter
        << ActionType::getShardMap
        << ActionType::hostInfo
        << ActionType::listDatabases
        << ActionType::listSessions // clusterManager gets this also
        << ActionType::listShards  // clusterManager gets this also
        << ActionType::netstat
        << ActionType::replSetGetConfig  // clusterManager gets this also
        << ActionType::replSetGetStatus  // clusterManager gets this also
        << ActionType::serverStatus 
        << ActionType::top
        << ActionType::useUUID
        << ActionType::inprog
        << ActionType::shardingState;

    // clusterMonitor role actions that target a database (or collection) resource
    clusterMonitorRoleDatabaseActions 
        << ActionType::collStats  // dbAdmin gets this also
        << ActionType::dbStats  // dbAdmin gets this also
        << ActionType::getDatabaseVersion
        << ActionType::getShardVersion
        << ActionType::indexStats;

    // hostManager role actions that target the cluster resource
    hostManagerRoleClusterActions
        << ActionType::applicationMessage  // clusterManager gets this also
        << ActionType::connPoolSync
        << ActionType::cpuProfiler
        << ActionType::dropConnections
        << ActionType::logRotate
        << ActionType::setParameter
        << ActionType::shutdown
        << ActionType::touch
        << ActionType::unlock
        << ActionType::flushRouterConfig  // clusterManager gets this also
        << ActionType::fsync
        << ActionType::invalidateUserCache // userAdminAnyDatabase gets this also
        << ActionType::killAnyCursor
        << ActionType::killAnySession
        << ActionType::killop
        << ActionType::replSetResizeOplog
        << ActionType::resync  // clusterManager gets this also
        << ActionType::trafficRecord;

    // hostManager role actions that target the database resource
    hostManagerRoleDatabaseActions
        << ActionType::killCursors;


    // clusterManager role actions that target the cluster resource
    clusterManagerRoleClusterActions
        << ActionType::appendOplogNote  // backup gets this also
        << ActionType::applicationMessage  // hostManager gets this also
        << ActionType::replSetConfigure
        << ActionType::replSetGetConfig  // clusterMonitor gets this also
        << ActionType::replSetGetStatus  // clusterMonitor gets this also
        << ActionType::replSetStateChange
        << ActionType::resync  // hostManager gets this also
        << ActionType::addShard 
        << ActionType::removeShard
        << ActionType::listSessions  // clusterMonitor gets this also
        << ActionType::listShards  // clusterMonitor gets this also
        << ActionType::flushRouterConfig  // hostManager gets this also
        << ActionType::cleanupOrphaned
        << ActionType::setFeatureCompatibilityVersion
        << ActionType::setFreeMonitoring;

    clusterManagerRoleDatabaseActions
        << ActionType::clearJumboFlag
        << ActionType::splitChunk
        << ActionType::moveChunk
        << ActionType::enableSharding
        << ActionType::splitVector;

    return Status::OK();
}