TEST(RoleGraphTest, AddRoleFromDocumentWithRestricitonMerge) {
    const BSONArray roleARestrictions = BSON_ARRAY(BSON("clientSource" << BSON_ARRAY("::1/128")));
    const BSONArray roleBRestrictions =
        BSON_ARRAY(BSON("serverAddress" << BSON_ARRAY("127.0.0.1/8")));

    RoleGraph graph;
    ASSERT_OK(graph.addRoleFromDocument(BSON("_id"
                                             << "dbA.roleA"
                                             << "role"
                                             << "roleA"
                                             << "db"
                                             << "dbA"
                                             << "privileges"
                                             << BSONArray()
                                             << "roles"
                                             << BSONArray()
                                             << "authenticationRestrictions"
                                             << roleARestrictions)));
    ASSERT_OK(graph.addRoleFromDocument(BSON("_id"
                                             << "dbB.roleB"
                                             << "role"
                                             << "roleB"
                                             << "db"
                                             << "dbB"
                                             << "privileges"
                                             << BSONArray()
                                             << "roles"
                                             << BSON_ARRAY(BSON("role"
                                                                << "roleA"
                                                                << "db"
                                                                << "dbA"))
                                             << "authenticationRestrictions"
                                             << roleBRestrictions)));
    ASSERT_OK(graph.recomputePrivilegeData());

    const auto A = graph.getDirectAuthenticationRestrictions(RoleName("roleA", "dbA"));
    const auto B = graph.getDirectAuthenticationRestrictions(RoleName("roleB", "dbB"));
    const auto gaar = graph.getAllAuthenticationRestrictions(RoleName("roleB", "dbB"));
    ASSERT_TRUE(std::any_of(gaar.begin(), gaar.end(), [A](const auto& r) { return A == r; }));
    ASSERT_TRUE(std::any_of(gaar.begin(), gaar.end(), [B](const auto& r) { return B == r; }));
}