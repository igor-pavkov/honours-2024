Status RoleGraph::removeAllRolesFromRole(const RoleName& victim) {
    typedef std::vector<RoleName> RoleNameVector;
    if (!roleExists(victim)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << victim.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(victim)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot remove roles from built-in role: "
                                                << victim.getFullName());
    }

    RoleNameVector& subordinatesOfVictim = _roleToSubordinates[victim];
    for (RoleNameVector::const_iterator subordinateRole = subordinatesOfVictim.begin(),
                                        end = subordinatesOfVictim.end();
         subordinateRole != end;
         ++subordinateRole) {
        RoleNameVector& membersOfSubordinate = _roleToMembers[*subordinateRole];
        RoleNameVector::iterator toErase =
            std::find(membersOfSubordinate.begin(), membersOfSubordinate.end(), victim);
        fassert(17173, toErase != membersOfSubordinate.end());
        membersOfSubordinate.erase(toErase);
    }
    subordinatesOfVictim.clear();
    return Status::OK();
}