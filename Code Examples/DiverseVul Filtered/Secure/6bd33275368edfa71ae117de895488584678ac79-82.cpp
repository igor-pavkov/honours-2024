static int mboxlist_have_admin_rights(const char *rights) {
    int access, have_admin_access;

    cyrus_acl_strtomask(rights, &access);
    have_admin_access = access & ACL_ADMIN;

    return have_admin_access;
}