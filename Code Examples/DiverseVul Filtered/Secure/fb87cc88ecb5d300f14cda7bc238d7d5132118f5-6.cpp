Status RoleGraph::_recomputePrivilegeDataHelper(const RoleName& startingRole,
                                                stdx::unordered_set<RoleName>& visitedRoles) {
    if (visitedRoles.count(startingRole)) {
        return Status::OK();
    }

    std::vector<RoleName> inProgressRoles;
    inProgressRoles.push_back(startingRole);
    while (inProgressRoles.size()) {
        const RoleName currentRole = inProgressRoles.back();
        fassert(17277, !visitedRoles.count(currentRole));

        if (!roleExists(currentRole)) {
            return Status(ErrorCodes::RoleNotFound,
                          mongoutils::str::stream() << "Role: " << currentRole.getFullName()
                                                    << " does not exist");
        }

        // Check for cycles
        {
            const std::vector<RoleName>::const_iterator begin = inProgressRoles.begin();
            // The currentRole will always be last so don't look there.
            const std::vector<RoleName>::const_iterator end = --inProgressRoles.end();
            const std::vector<RoleName>::const_iterator firstOccurence =
                std::find(begin, end, currentRole);
            if (firstOccurence != end) {
                std::ostringstream os;
                os << "Cycle in dependency graph: ";
                for (std::vector<RoleName>::const_iterator it = firstOccurence; it != end; ++it) {
                    os << it->getFullName() << " -> ";
                }
                os << currentRole.getFullName();
                return Status(ErrorCodes::GraphContainsCycle, os.str());
            }
        }

        // Make sure we've already visited all subordinate roles before worrying about this one.
        const std::vector<RoleName>& currentRoleDirectRoles = _roleToSubordinates[currentRole];
        std::vector<RoleName>::const_iterator roleIt;
        for (roleIt = currentRoleDirectRoles.begin(); roleIt != currentRoleDirectRoles.end();
             ++roleIt) {
            const RoleName& childRole = *roleIt;
            if (!visitedRoles.count(childRole)) {
                inProgressRoles.push_back(childRole);
                break;
            }
        }
        // If roleIt didn't reach the end of currentRoleDirectRoles that means we found a child
        // of currentRole that we haven't visited yet.
        if (roleIt != currentRoleDirectRoles.end()) {
            continue;
        }
        // At this point, we know that we've already visited all child roles of currentRole
        // and thus their "all privileges" sets are correct and can be added to currentRole's
        // "all privileges" set

        // Need to clear out the "all privileges" vector for the current role, and re-fill it
        // with just the direct privileges for this role.
        PrivilegeVector& currentRoleAllPrivileges = _allPrivilegesForRole[currentRole];
        currentRoleAllPrivileges = _directPrivilegesForRole[currentRole];

        // Need to do the same thing for the indirect roles
        stdx::unordered_set<RoleName>& currentRoleIndirectRoles =
            _roleToIndirectSubordinates[currentRole];
        currentRoleIndirectRoles.clear();
        for (const auto& role : currentRoleDirectRoles) {
            currentRoleIndirectRoles.insert(role);
        }

        // Also clear the "all restrictions" to rebuild in loop
        auto& currentRoleAllRestrictions = _allRestrictionsForRole[currentRole];
        currentRoleAllRestrictions.clear();
        auto& currentRoleDirectRestrictions = _directRestrictionsForRole[currentRole];
        if (currentRoleDirectRestrictions) {
            currentRoleAllRestrictions.push_back(currentRoleDirectRestrictions);
        }

        // Recursively add children's privileges to current role's "all privileges" vector, and
        // children's roles to current roles's "indirect roles" vector.
        for (const auto& childRole : currentRoleDirectRoles) {
            // At this point, we already know that the "all privilege" set for the child is
            // correct, so add those privileges to our "all privilege" set.

            for (const auto& priv : _allPrivilegesForRole[childRole]) {
                Privilege::addPrivilegeToPrivilegeVector(&currentRoleAllPrivileges, priv);
            }

            // We also know that the "indirect roles" for the child is also correct, so we can
            // add those roles to our "indirect roles" set.
            const auto& childAllRolesToIndirectSubordinates =
                _roleToIndirectSubordinates[childRole];
            currentRoleIndirectRoles.insert(childAllRolesToIndirectSubordinates.begin(),
                                            childAllRolesToIndirectSubordinates.end());

            // Similarly, "indirect restrictions" are ready to append
            const auto& childAllRestrictionsForRole = _allRestrictionsForRole[childRole];
            currentRoleAllRestrictions.insert(currentRoleAllRestrictions.end(),
                                              childAllRestrictionsForRole.begin(),
                                              childAllRestrictionsForRole.end());
        }

        visitedRoles.insert(currentRole);
        inProgressRoles.pop_back();
    }
    return Status::OK();
}