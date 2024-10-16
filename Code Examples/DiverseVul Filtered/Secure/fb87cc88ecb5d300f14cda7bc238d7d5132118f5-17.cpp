Status RoleGraph::replaceRole(const RoleName& roleName,
                              const std::vector<RoleName>& roles,
                              const PrivilegeVector& privileges,
                              SharedRestrictionDocument restrictions) {
    Status status = removeAllPrivilegesFromRole(roleName);
    if (status == ErrorCodes::RoleNotFound) {
        fassert(17168, createRole(roleName));
    } else if (!status.isOK()) {
        return status;
    }
    fassert(17169, removeAllRolesFromRole(roleName));
    fassert(40556, replaceRestrictionsForRole(roleName, restrictions));
    for (size_t i = 0; i < roles.size(); ++i) {
        const RoleName& grantedRole = roles[i];
        status = createRole(grantedRole);
        fassert(17170, status.isOK() || status == ErrorCodes::DuplicateKey);
        fassert(17171, addRoleToRole(roleName, grantedRole));
    }
    fassert(17172, addPrivilegesToRole(roleName, privileges));
    return Status::OK();
}