RunOpenvpn(LPVOID p)
{
    HANDLE pipe = p;
    HANDLE ovpn_pipe, svc_pipe;
    PTOKEN_USER svc_user, ovpn_user;
    HANDLE svc_token = NULL, imp_token = NULL, pri_token = NULL;
    HANDLE stdin_read = NULL, stdin_write = NULL;
    HANDLE stdout_write = NULL;
    DWORD pipe_mode, len, exit_code = 0;
    STARTUP_DATA sud = { 0, 0, 0 };
    STARTUPINFOW startup_info;
    PROCESS_INFORMATION proc_info;
    LPVOID user_env = NULL;
    TCHAR ovpn_pipe_name[256]; /* The entire pipe name string can be up to 256 characters long according to MSDN. */
    LPCWSTR exe_path;
    WCHAR *cmdline = NULL;
    size_t cmdline_size;
    undo_lists_t undo_lists;

    SECURITY_ATTRIBUTES inheritable = {
        .nLength = sizeof(inheritable),
        .lpSecurityDescriptor = NULL,
        .bInheritHandle = TRUE
    };

    PACL ovpn_dacl;
    EXPLICIT_ACCESS ea[2];
    SECURITY_DESCRIPTOR ovpn_sd;
    SECURITY_ATTRIBUTES ovpn_sa = {
        .nLength = sizeof(ovpn_sa),
        .lpSecurityDescriptor = &ovpn_sd,
        .bInheritHandle = FALSE
    };

    ZeroMemory(&ea, sizeof(ea));
    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&undo_lists, sizeof(undo_lists));
    ZeroMemory(&proc_info, sizeof(proc_info));

    if (!GetStartupData(pipe, &sud))
    {
        goto out;
    }

    if (!InitializeSecurityDescriptor(&ovpn_sd, SECURITY_DESCRIPTOR_REVISION))
    {
        ReturnLastError(pipe, L"InitializeSecurityDescriptor");
        goto out;
    }

    /* Get SID of user the service is running under */
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &svc_token))
    {
        ReturnLastError(pipe, L"OpenProcessToken");
        goto out;
    }
    len = 0;
    svc_user = NULL;
    while (!GetTokenInformation(svc_token, TokenUser, svc_user, len, &len))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            ReturnLastError(pipe, L"GetTokenInformation (service token)");
            goto out;
        }
        free(svc_user);
        svc_user = malloc(len);
        if (svc_user == NULL)
        {
            ReturnLastError(pipe, L"malloc (service token user)");
            goto out;
        }
    }
    if (!IsValidSid(svc_user->User.Sid))
    {
        ReturnLastError(pipe, L"IsValidSid (service token user)");
        goto out;
    }

    if (!ImpersonateNamedPipeClient(pipe))
    {
        ReturnLastError(pipe, L"ImpersonateNamedPipeClient");
        goto out;
    }
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE, &imp_token))
    {
        ReturnLastError(pipe, L"OpenThreadToken");
        goto out;
    }
    len = 0;
    ovpn_user = NULL;
    while (!GetTokenInformation(imp_token, TokenUser, ovpn_user, len, &len))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            ReturnLastError(pipe, L"GetTokenInformation (impersonation token)");
            goto out;
        }
        free(ovpn_user);
        ovpn_user = malloc(len);
        if (ovpn_user == NULL)
        {
            ReturnLastError(pipe, L"malloc (impersonation token user)");
            goto out;
        }
    }
    if (!IsValidSid(ovpn_user->User.Sid))
    {
        ReturnLastError(pipe, L"IsValidSid (impersonation token user)");
        goto out;
    }

    /* Check user is authorized or options are white-listed */
    if (!IsAuthorizedUser(ovpn_user->User.Sid, imp_token, settings.ovpn_admin_group)
        && !ValidateOptions(pipe, sud.directory, sud.options))
    {
        goto out;
    }

    /* OpenVPN process DACL entry for access by service and user */
    ea[0].grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
    ea[0].Trustee.ptstrName = (LPTSTR) svc_user->User.Sid;
    ea[1].grfAccessPermissions = READ_CONTROL | SYNCHRONIZE | PROCESS_VM_READ
                                 |SYNCHRONIZE | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfInheritance = NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
    ea[1].Trustee.ptstrName = (LPTSTR) ovpn_user->User.Sid;

    /* Set owner and DACL of OpenVPN security descriptor */
    if (!SetSecurityDescriptorOwner(&ovpn_sd, svc_user->User.Sid, FALSE))
    {
        ReturnLastError(pipe, L"SetSecurityDescriptorOwner");
        goto out;
    }
    if (SetEntriesInAcl(2, ea, NULL, &ovpn_dacl) != ERROR_SUCCESS)
    {
        ReturnLastError(pipe, L"SetEntriesInAcl");
        goto out;
    }
    if (!SetSecurityDescriptorDacl(&ovpn_sd, TRUE, ovpn_dacl, FALSE))
    {
        ReturnLastError(pipe, L"SetSecurityDescriptorDacl");
        goto out;
    }

    /* Create primary token from impersonation token */
    if (!DuplicateTokenEx(imp_token, TOKEN_ALL_ACCESS, NULL, 0, TokenPrimary, &pri_token))
    {
        ReturnLastError(pipe, L"DuplicateTokenEx");
        goto out;
    }

    /* use /dev/null for stdout of openvpn (client should use --log for output) */
    stdout_write = CreateFile(_T("NUL"), GENERIC_WRITE, FILE_SHARE_WRITE,
                              &inheritable, OPEN_EXISTING, 0, NULL);
    if (stdout_write == INVALID_HANDLE_VALUE)
    {
        ReturnLastError(pipe, L"CreateFile for stdout");
        goto out;
    }

    if (!CreatePipe(&stdin_read, &stdin_write, &inheritable, 0)
        || !SetHandleInformation(stdin_write, HANDLE_FLAG_INHERIT, 0))
    {
        ReturnLastError(pipe, L"CreatePipe");
        goto out;
    }

    openvpn_sntprintf(ovpn_pipe_name, _countof(ovpn_pipe_name),
                      TEXT("\\\\.\\pipe\\" PACKAGE "%s\\service_%lu"), service_instance, GetCurrentThreadId());
    ovpn_pipe = CreateNamedPipe(ovpn_pipe_name,
                                PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 128, 128, 0, NULL);
    if (ovpn_pipe == INVALID_HANDLE_VALUE)
    {
        ReturnLastError(pipe, L"CreateNamedPipe");
        goto out;
    }

    svc_pipe = CreateFile(ovpn_pipe_name, GENERIC_READ | GENERIC_WRITE, 0,
                          &inheritable, OPEN_EXISTING, 0, NULL);
    if (svc_pipe == INVALID_HANDLE_VALUE)
    {
        ReturnLastError(pipe, L"CreateFile");
        goto out;
    }

    pipe_mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(svc_pipe, &pipe_mode, NULL, NULL))
    {
        ReturnLastError(pipe, L"SetNamedPipeHandleState");
        goto out;
    }

    cmdline_size = wcslen(sud.options) + 128;
    cmdline = malloc(cmdline_size * sizeof(*cmdline));
    if (cmdline == NULL)
    {
        ReturnLastError(pipe, L"malloc");
        goto out;
    }
    openvpn_sntprintf(cmdline, cmdline_size, L"openvpn %s --msg-channel %lu",
                      sud.options, svc_pipe);

    if (!CreateEnvironmentBlock(&user_env, imp_token, FALSE))
    {
        ReturnLastError(pipe, L"CreateEnvironmentBlock");
        goto out;
    }

    startup_info.cb = sizeof(startup_info);
    startup_info.lpDesktop = L"winsta0\\default";
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = stdin_read;
    startup_info.hStdOutput = stdout_write;
    startup_info.hStdError = stdout_write;

#ifdef UNICODE
    exe_path = settings.exe_path;
#else
    WCHAR wide_path[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, settings.exe_path, MAX_PATH, wide_path, MAX_PATH);
    exe_path = wide_path;
#endif

    /* TODO: make sure HKCU is correct or call LoadUserProfile() */
    if (!CreateProcessAsUserW(pri_token, exe_path, cmdline, &ovpn_sa, NULL, TRUE,
                              settings.priority | CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
                              user_env, sud.directory, &startup_info, &proc_info))
    {
        ReturnLastError(pipe, L"CreateProcessAsUser");
        goto out;
    }

    if (!RevertToSelf())
    {
        TerminateProcess(proc_info.hProcess, 1);
        ReturnLastError(pipe, L"RevertToSelf");
        goto out;
    }

    ReturnProcessId(pipe, proc_info.dwProcessId, 1, &exit_event);

    CloseHandleEx(&stdout_write);
    CloseHandleEx(&stdin_read);
    CloseHandleEx(&svc_pipe);

    DWORD input_size = WideCharToMultiByte(CP_UTF8, 0, sud.std_input, -1, NULL, 0, NULL, NULL);
    LPSTR input = NULL;
    if (input_size && (input = malloc(input_size)))
    {
        DWORD written;
        WideCharToMultiByte(CP_UTF8, 0, sud.std_input, -1, input, input_size, NULL, NULL);
        WriteFile(stdin_write, input, (DWORD)strlen(input), &written, NULL);
        free(input);
    }

    while (TRUE)
    {
        DWORD bytes = PeekNamedPipeAsync(ovpn_pipe, 1, &exit_event);
        if (bytes == 0)
        {
            break;
        }

        HandleMessage(ovpn_pipe, bytes, 1, &exit_event, &undo_lists);
    }

    WaitForSingleObject(proc_info.hProcess, IO_TIMEOUT);
    GetExitCodeProcess(proc_info.hProcess, &exit_code);
    if (exit_code == STILL_ACTIVE)
    {
        TerminateProcess(proc_info.hProcess, 1);
    }
    else if (exit_code != 0)
    {
        WCHAR buf[256];
        swprintf(buf, _countof(buf),
                 L"OpenVPN exited with error: exit code = %lu", exit_code);
        buf[_countof(buf) - 1] =  L'\0';
        ReturnError(pipe, ERROR_OPENVPN_STARTUP, buf, 1, &exit_event);
    }
    Undo(&undo_lists);

out:
    FlushFileBuffers(pipe);
    DisconnectNamedPipe(pipe);

    free(ovpn_user);
    free(svc_user);
    free(cmdline);
    DestroyEnvironmentBlock(user_env);
    FreeStartupData(&sud);
    CloseHandleEx(&proc_info.hProcess);
    CloseHandleEx(&proc_info.hThread);
    CloseHandleEx(&stdin_read);
    CloseHandleEx(&stdin_write);
    CloseHandleEx(&stdout_write);
    CloseHandleEx(&svc_token);
    CloseHandleEx(&imp_token);
    CloseHandleEx(&pri_token);
    CloseHandleEx(&ovpn_pipe);
    CloseHandleEx(&svc_pipe);
    CloseHandleEx(&pipe);

    return 0;
}