TEST(RoleGraphTest, AddRoleFromDocument) {
    const BSONArray roles[] =
        {
            BSONArray(),
            BSON_ARRAY(BSON("role"
                            << "roleA"
                            << "db"
                            << "dbA")),
            BSON_ARRAY(BSON("role"
                            << "roleB"
                            << "db"
                            << "dbB")),
            BSON_ARRAY(BSON("role"
                            << "roleA"
                            << "db"
                            << "dbA")
                       << BSON("role"
                               << "roleB"
                               << "db"
                               << "dbB")),
        };

    const BSONArray privs[] = {
        BSONArray(),
        BSON_ARRAY(BSON("resource" << BSON("db"
                                           << "dbA"
                                           << "collection"
                                           << "collA")
                                   << "actions"
                                   << BSON_ARRAY("insert"))),
        BSON_ARRAY(BSON("resource" << BSON("db"
                                           << "dbB"
                                           << "collection"
                                           << "collB")
                                   << "actions"
                                   << BSON_ARRAY("insert"))
                   << BSON("resource" << BSON("db"
                                              << "dbC"
                                              << "collection"
                                              << "collC")
                                      << "actions"
                                      << BSON_ARRAY("compact"))),
        BSON_ARRAY(BSON("resource" << BSON("db"
                                           << ""
                                           << "collection"
                                           << "")
                                   << "actions"
                                   << BSON_ARRAY("find"))),
    };

    const BSONArray restrictions[] = {
        BSONArray(),
        BSON_ARRAY(BSON("clientSource" << BSON_ARRAY("::1/128"
                                                     << "127.0.0.1/8"))),
        BSON_ARRAY(BSON("clientSource" << BSON_ARRAY("::1/128"))
                   << BSON("clientSource" << BSON_ARRAY("127.0.0.1/8"))),
        BSON_ARRAY(BSON("clientSource" << BSON_ARRAY("::1/128") << "serverAddress"
                                       << BSON_ARRAY("::1/128"))),
    };

    const auto dummyRoleDoc = [](const std::string& role,
                                 const std::string& db,
                                 const BSONArray& privs,
                                 const BSONArray& roles,
                                 const boost::optional<BSONArray>& restriction) {
        BSONObjBuilder builder;
        builder.append("_id", db + "." + role);
        builder.append("role", role);
        builder.append("db", db);
        builder.append("privileges", privs);
        builder.append("roles", roles);
        if (restriction) {
            builder.append("authenticationRestrictions", restriction.get());
        }
        return builder.obj();
    };

    RoleGraph graph;

    // roleA is empty, roleB contains roleA, roleC contains roleB (and by extension roleA)
    ASSERT_OK(graph.addRoleFromDocument(
        dummyRoleDoc("roleA", "dbA", privs[0], roles[0], restrictions[0])));
    ASSERT_OK(graph.addRoleFromDocument(
        dummyRoleDoc("roleB", "dbB", privs[1], roles[1], restrictions[1])));
    ASSERT_OK(graph.addRoleFromDocument(
        dummyRoleDoc("roleC", "dbC", privs[2], roles[2], restrictions[2])));
    ASSERT_OK(
        graph.addRoleFromDocument(dummyRoleDoc("roleD", "dbD", privs[3], roles[3], boost::none)));

    const RoleName tmpRole("roleE", "dbE");
    for (const auto& priv : privs) {
        for (const auto& role : roles) {
            ASSERT_OK(graph.addRoleFromDocument(dummyRoleDoc(tmpRole.getRole().toString(),
                                                             tmpRole.getDB().toString(),
                                                             priv,
                                                             role,
                                                             boost::none)));
            ASSERT_OK(graph.recomputePrivilegeData());
            ASSERT_FALSE(graph.getDirectAuthenticationRestrictions(tmpRole));
            ASSERT_OK(graph.deleteRole(tmpRole));

            for (const auto& restriction : restrictions) {
                ASSERT_OK(graph.addRoleFromDocument(dummyRoleDoc(tmpRole.getRole().toString(),
                                                                 tmpRole.getDB().toString(),
                                                                 priv,
                                                                 role,
                                                                 restriction)));
                ASSERT_OK(graph.recomputePrivilegeData());
                const auto current = graph.getDirectAuthenticationRestrictions(tmpRole);
                ASSERT_BSONOBJ_EQ(current->toBSON(), restriction);
                const auto gaar = graph.getAllAuthenticationRestrictions(tmpRole);
                ASSERT_TRUE(std::any_of(
                    gaar.begin(), gaar.end(), [current](const auto& r) { return current == r; }));
                ASSERT_OK(graph.deleteRole(tmpRole));
            }
        }
    }
}