    DWORD FileIo::Impl::winNumberOfLinks() const
    {
        DWORD nlink = 1;

        HANDLE hFd = (HANDLE)_get_osfhandle(fileno(fp_));
        if (hFd != INVALID_HANDLE_VALUE) {
            typedef BOOL (WINAPI * GetFileInformationByHandle_t)(HANDLE, LPBY_HANDLE_FILE_INFORMATION);
            HMODULE hKernel = ::GetModuleHandleA("kernel32.dll");
            if (hKernel) {
                GetFileInformationByHandle_t pfcn_GetFileInformationByHandle = (GetFileInformationByHandle_t)GetProcAddress(hKernel, "GetFileInformationByHandle");
                if (pfcn_GetFileInformationByHandle) {
                    BY_HANDLE_FILE_INFORMATION fi = {0,0,0,0,0,0,0,0,0,0,0,0,0};
                    if (pfcn_GetFileInformationByHandle(hFd, &fi)) {
                        nlink = fi.nNumberOfLinks;
                    }
#ifdef EXIV2_DEBUG_MESSAGES
                    else EXV_DEBUG << "GetFileInformationByHandle failed\n";
#endif
                }
#ifdef EXIV2_DEBUG_MESSAGES
                else EXV_DEBUG << "GetProcAddress(hKernel, \"GetFileInformationByHandle\") failed\n";
#endif
            }
#ifdef EXIV2_DEBUG_MESSAGES
            else EXV_DEBUG << "GetModuleHandleA(\"kernel32.dll\") failed\n";
#endif
        }
#ifdef EXIV2_DEBUG_MESSAGES
        else EXV_DEBUG << "_get_osfhandle failed: INVALID_HANDLE_VALUE\n";
#endif

        return nlink;
    } // FileIo::Impl::winNumberOfLinks