Status RoleGraph::removePrivilegeFromRole(const RoleName& role,
                                          const Privilege& privilegeToRemove) {
    if (!roleExists(role)) {
        return Status(ErrorCodes::RoleNotFound,
                      mongoutils::str::stream() << "Role: " << role.getFullName()
                                                << " does not exist");
    }
    if (isBuiltinRole(role)) {
        return Status(ErrorCodes::InvalidRoleModification,
                      mongoutils::str::stream() << "Cannot remove privileges from built-in role: "
                                                << role.getFullName());
    }

    PrivilegeVector& currentPrivileges = _directPrivilegesForRole[role];
    for (PrivilegeVector::iterator it = currentPrivileges.begin(); it != currentPrivileges.end();
         ++it) {
        Privilege& curPrivilege = *it;
        if (curPrivilege.getResourcePattern() == privilegeToRemove.getResourcePattern()) {
            ActionSet curActions = curPrivilege.getActions();

            if (!curActions.isSupersetOf(privilegeToRemove.getActions())) {
                // Didn't possess all the actions being removed.
                return Status(
                    ErrorCodes::PrivilegeNotFound,
                    mongoutils::str::stream() << "Role: " << role.getFullName()
                                              << " does not contain a privilege on "
                                              << privilegeToRemove.getResourcePattern().toString()
                                              << " with actions: "
                                              << privilegeToRemove.getActions().toString());
            }

            curPrivilege.removeActions(privilegeToRemove.getActions());
            if (curPrivilege.getActions().empty()) {
                currentPrivileges.erase(it);
            }
            return Status::OK();
        }
    }
    return Status(ErrorCodes::PrivilegeNotFound,
                  mongoutils::str::stream() << "Role: " << role.getFullName()
                                            << " does not "
                                               "contain any privileges on "
                                            << privilegeToRemove.getResourcePattern().toString());
}