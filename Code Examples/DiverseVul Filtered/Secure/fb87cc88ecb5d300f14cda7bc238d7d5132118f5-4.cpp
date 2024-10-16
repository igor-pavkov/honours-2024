TEST(RoleGraphTest, AddRemoveRoles) {
    RoleName roleA("roleA", "dbA");
    RoleName roleB("roleB", "dbB");
    RoleName roleC("roleC", "dbC");
    RoleName roleD("readWrite", "dbD");  // built-in role

    RoleGraph graph;
    ASSERT_OK(graph.createRole(roleA));
    ASSERT_OK(graph.createRole(roleB));
    ASSERT_OK(graph.createRole(roleC));

    RoleNameIterator it;
    it = graph.getDirectSubordinates(roleA);
    ASSERT_FALSE(it.more());
    it = graph.getDirectMembers(roleA);
    ASSERT_FALSE(it.more());

    ASSERT_OK(graph.addRoleToRole(roleA, roleB));

    // A -> B
    it = graph.getDirectSubordinates(roleA);
    ASSERT_TRUE(it.more());
    // should not advance the iterator
    ASSERT_EQUALS(it.get().getFullName(), roleB.getFullName());
    ASSERT_EQUALS(it.get().getFullName(), roleB.getFullName());
    ASSERT_EQUALS(it.next().getFullName(), roleB.getFullName());
    ASSERT_FALSE(it.more());

    it = graph.getDirectMembers(roleA);
    ASSERT_FALSE(it.more());

    it = graph.getDirectMembers(roleB);
    ASSERT_EQUALS(it.next().getFullName(), roleA.getFullName());
    ASSERT_FALSE(it.more());

    it = graph.getDirectSubordinates(roleB);
    ASSERT_FALSE(it.more());

    ASSERT_OK(graph.addRoleToRole(roleA, roleC));
    ASSERT_OK(graph.addRoleToRole(roleB, roleC));
    ASSERT_OK(graph.addRoleToRole(roleB, roleD));
    // Adding the same role twice should be a no-op, duplicate roles should be de-duped.
    ASSERT_OK(graph.addRoleToRole(roleB, roleD));

    /*
     * Graph now looks like:
     *   A
     *  / \
     * v   v
     * B -> C
     * |
     * v
     * D
    */


    it = graph.getDirectSubordinates(roleA);  // should be roleB and roleC, order doesn't matter
    RoleName cur = it.next();
    if (cur == roleB) {
        ASSERT_EQUALS(it.next().getFullName(), roleC.getFullName());
    } else if (cur == roleC) {
        ASSERT_EQUALS(it.next().getFullName(), roleB.getFullName());
    } else {
        FAIL(mongoutils::str::stream() << "unexpected role returned: " << cur.getFullName());
    }
    ASSERT_FALSE(it.more());

    ASSERT_OK(graph.recomputePrivilegeData());
    it = graph.getIndirectSubordinates(roleA);  // should have roleB, roleC and roleD
    bool hasB = false;
    bool hasC = false;
    bool hasD = false;
    int num = 0;
    while (it.more()) {
        ++num;
        RoleName cur = it.next();
        if (cur == roleB) {
            hasB = true;
        } else if (cur == roleC) {
            hasC = true;
        } else if (cur == roleD) {
            hasD = true;
        } else {
            FAIL(mongoutils::str::stream() << "unexpected role returned: " << cur.getFullName());
        }
    }
    ASSERT_EQUALS(3, num);
    ASSERT(hasB);
    ASSERT(hasC);
    ASSERT(hasD);

    it = graph.getDirectSubordinates(roleB);  // should be roleC and roleD, order doesn't matter
    cur = it.next();
    if (cur == roleC) {
        ASSERT_EQUALS(it.next().getFullName(), roleD.getFullName());
    } else if (cur == roleD) {
        ASSERT_EQUALS(it.next().getFullName(), roleC.getFullName());
    } else {
        FAIL(mongoutils::str::stream() << "unexpected role returned: " << cur.getFullName());
    }
    ASSERT_FALSE(it.more());

    it = graph.getDirectSubordinates(roleC);
    ASSERT_FALSE(it.more());

    it = graph.getDirectMembers(roleA);
    ASSERT_FALSE(it.more());

    it = graph.getDirectMembers(roleB);
    ASSERT_EQUALS(it.next().getFullName(), roleA.getFullName());
    ASSERT_FALSE(it.more());

    it = graph.getDirectMembers(roleC);  // should be role A and role B, order doesn't matter
    cur = it.next();
    if (cur == roleA) {
        ASSERT_EQUALS(it.next().getFullName(), roleB.getFullName());
    } else if (cur == roleB) {
        ASSERT_EQUALS(it.next().getFullName(), roleA.getFullName());
    } else {
        FAIL(mongoutils::str::stream() << "unexpected role returned: " << cur.getFullName());
    }
    ASSERT_FALSE(it.more());

    // Now remove roleD from roleB and make sure graph is update correctly
    ASSERT_OK(graph.removeRoleFromRole(roleB, roleD));

    /*
     * Graph now looks like:
     *   A
     *  / \
     * v   v
     * B -> C
     */
    it = graph.getDirectSubordinates(roleB);  // should be just roleC
    ASSERT_EQUALS(it.next().getFullName(), roleC.getFullName());
    ASSERT_FALSE(it.more());

    it = graph.getDirectSubordinates(roleD);  // should be empty
    ASSERT_FALSE(it.more());


    // Now delete roleB entirely and make sure that the other roles are updated properly
    ASSERT_OK(graph.deleteRole(roleB));
    ASSERT_NOT_OK(graph.deleteRole(roleB));
    it = graph.getDirectSubordinates(roleA);
    ASSERT_EQUALS(it.next().getFullName(), roleC.getFullName());
    ASSERT_FALSE(it.more());
    it = graph.getDirectMembers(roleC);
    ASSERT_EQUALS(it.next().getFullName(), roleA.getFullName());
    ASSERT_FALSE(it.more());
}