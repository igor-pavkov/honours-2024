CreateClientPipeInstance(VOID)
{
    TCHAR pipe_name[256]; /* The entire pipe name string can be up to 256 characters long according to MSDN. */
    HANDLE pipe = NULL;
    PACL old_dacl, new_dacl;
    PSECURITY_DESCRIPTOR sd;
    static EXPLICIT_ACCESS ea[2];
    static BOOL initialized = FALSE;
    DWORD flags = PIPE_ACCESS_DUPLEX | WRITE_DAC | FILE_FLAG_OVERLAPPED;

    if (!initialized)
    {
        PSID everyone, anonymous;

        ConvertStringSidToSid(TEXT("S-1-1-0"), &everyone);
        ConvertStringSidToSid(TEXT("S-1-5-7"), &anonymous);

        ea[0].grfAccessPermissions = FILE_GENERIC_WRITE;
        ea[0].grfAccessMode = GRANT_ACCESS;
        ea[0].grfInheritance = NO_INHERITANCE;
        ea[0].Trustee.pMultipleTrustee = NULL;
        ea[0].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
        ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea[0].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
        ea[0].Trustee.ptstrName = (LPTSTR) everyone;

        ea[1].grfAccessPermissions = 0;
        ea[1].grfAccessMode = REVOKE_ACCESS;
        ea[1].grfInheritance = NO_INHERITANCE;
        ea[1].Trustee.pMultipleTrustee = NULL;
        ea[1].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
        ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea[1].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
        ea[1].Trustee.ptstrName = (LPTSTR) anonymous;

        flags |= FILE_FLAG_FIRST_PIPE_INSTANCE;
        initialized = TRUE;
    }

    openvpn_sntprintf(pipe_name, _countof(pipe_name), TEXT("\\\\.\\pipe\\" PACKAGE "%s\\service"), service_instance);
    pipe = CreateNamedPipe(pipe_name, flags,
                           PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                           PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        MsgToEventLog(M_SYSERR, TEXT("Could not create named pipe"));
        return INVALID_HANDLE_VALUE;
    }

    if (GetSecurityInfo(pipe, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                        NULL, NULL, &old_dacl, NULL, &sd) != ERROR_SUCCESS)
    {
        MsgToEventLog(M_SYSERR, TEXT("Could not get pipe security info"));
        return CloseHandleEx(&pipe);
    }

    if (SetEntriesInAcl(2, ea, old_dacl, &new_dacl) != ERROR_SUCCESS)
    {
        MsgToEventLog(M_SYSERR, TEXT("Could not set entries in new acl"));
        return CloseHandleEx(&pipe);
    }

    if (SetSecurityInfo(pipe, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                        NULL, NULL, new_dacl, NULL) != ERROR_SUCCESS)
    {
        MsgToEventLog(M_SYSERR, TEXT("Could not set pipe security info"));
        return CloseHandleEx(&pipe);
    }

    return pipe;
}