rfbBool rfbSendDirContent(rfbClientPtr cl, int length, char *buffer)
{
    char retfilename[MAX_PATH];
    char path[MAX_PATH];
    struct stat statbuf;
    RFB_FIND_DATA win32filename;
    int nOptLen = 0, retval=0;
#ifdef WIN32
    WIN32_FIND_DATAA winFindData;
    HANDLE findHandle;
    int pathLen, basePathLength;
    char *basePath;
#else
    DIR *dirp=NULL;
    struct dirent *direntp=NULL;
#endif

    FILEXFER_ALLOWED_OR_CLOSE_AND_RETURN("", cl, FALSE);

    /* Client thinks we are Winblows */
    if (!rfbFilenameTranslate2UNIX(cl, buffer, path, sizeof(path)))
        return FALSE;

    if (DB) rfbLog("rfbProcessFileTransfer() rfbDirContentRequest: rfbRDirContent: \"%s\"->\"%s\"\n",buffer, path);

#ifdef WIN32
    // Create a search string, like C:\folder\*

    pathLen = strlen(path);
    basePath = malloc(pathLen + 3);
    memcpy(basePath, path, pathLen);
    basePathLength = pathLen;
    basePath[basePathLength] = '\\';
    basePath[basePathLength + 1] = '*';
    basePath[basePathLength + 2] = '\0';

    // Start a search
    memset(&winFindData, 0, sizeof(winFindData));
    findHandle = FindFirstFileA(path, &winFindData);
    free(basePath);

    if (findHandle == INVALID_HANDLE_VALUE)
#else
    dirp=opendir(path);
    if (dirp==NULL)
#endif
        return rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, 0, NULL);

    /* send back the path name (necessary for links) */
    if (rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, length, buffer)==FALSE) return FALSE;

#ifdef WIN32
    while (findHandle != INVALID_HANDLE_VALUE)
#else
    for (direntp=readdir(dirp); direntp!=NULL; direntp=readdir(dirp))
#endif
    {
        /* get stats */
#ifdef WIN32
    snprintf(retfilename,sizeof(retfilename),"%s/%s", path, winFindData.cFileName);
#else
    snprintf(retfilename,sizeof(retfilename),"%s/%s", path, direntp->d_name);
#endif
        retval = stat(retfilename, &statbuf);

        if (retval==0)
        {
            memset((char *)&win32filename, 0, sizeof(win32filename));
#ifdef WIN32
            win32filename.dwFileAttributes = winFindData.dwFileAttributes;
            win32filename.ftCreationTime.dwLowDateTime = winFindData.ftCreationTime.dwLowDateTime;
            win32filename.ftCreationTime.dwHighDateTime = winFindData.ftCreationTime.dwHighDateTime;
            win32filename.ftLastAccessTime.dwLowDateTime = winFindData.ftLastAccessTime.dwLowDateTime;
            win32filename.ftLastAccessTime.dwHighDateTime = winFindData.ftLastAccessTime.dwHighDateTime;
            win32filename.ftLastWriteTime.dwLowDateTime = winFindData.ftLastWriteTime.dwLowDateTime;
            win32filename.ftLastWriteTime.dwHighDateTime = winFindData.ftLastWriteTime.dwHighDateTime;
            win32filename.nFileSizeLow = winFindData.nFileSizeLow;
            win32filename.nFileSizeHigh = winFindData.nFileSizeHigh;
            win32filename.dwReserved0 = winFindData.dwReserved0;
            win32filename.dwReserved1 = winFindData.dwReserved1;
            strcpy((char *)win32filename.cFileName, winFindData.cFileName);
            strcpy((char *)win32filename.cAlternateFileName, winFindData.cAlternateFileName);
#else
            win32filename.dwFileAttributes = Swap32IfBE(RFB_FILE_ATTRIBUTE_NORMAL);
            if (S_ISDIR(statbuf.st_mode))
                win32filename.dwFileAttributes = Swap32IfBE(RFB_FILE_ATTRIBUTE_DIRECTORY);
            win32filename.ftCreationTime.dwLowDateTime = Swap32IfBE(statbuf.st_ctime);   /* Intel Order */
            win32filename.ftCreationTime.dwHighDateTime = 0;
            win32filename.ftLastAccessTime.dwLowDateTime = Swap32IfBE(statbuf.st_atime); /* Intel Order */
            win32filename.ftLastAccessTime.dwHighDateTime = 0;
            win32filename.ftLastWriteTime.dwLowDateTime = Swap32IfBE(statbuf.st_mtime);  /* Intel Order */
            win32filename.ftLastWriteTime.dwHighDateTime = 0;
            win32filename.nFileSizeLow = Swap32IfBE(statbuf.st_size); /* Intel Order */
            win32filename.nFileSizeHigh = 0;
            win32filename.dwReserved0 = 0;
            win32filename.dwReserved1 = 0;

            /* If this had the full path, we would need to translate to DOS format ("C:\") */
            /* rfbFilenameTranslate2DOS(cl, retfilename, win32filename.cFileName); */
            strcpy((char *)win32filename.cFileName, direntp->d_name);
#endif
            
            /* Do not show hidden files (but show how to move up the tree) */
            if ((strcmp((char *)win32filename.cFileName, "..")==0) || (win32filename.cFileName[0]!='.'))
            {
                nOptLen = sizeof(RFB_FIND_DATA) - MAX_PATH - 14 + strlen((char *)win32filename.cFileName);
                /*
                rfbLog("rfbProcessFileTransfer() rfbDirContentRequest: rfbRDirContent: Sending \"%s\"\n", (char *)win32filename.cFileName);
                */
                if (rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, nOptLen, (char *)&win32filename)==FALSE)
                {
#ifdef WIN32
                    FindClose(findHandle);
#else
                    closedir(dirp);
#endif
                    return FALSE;
                }
            }
        }

#ifdef WIN32
        if (FindNextFileA(findHandle, &winFindData) == 0)
        {
            FindClose(findHandle);
            findHandle = INVALID_HANDLE_VALUE;
        }
#endif
    }
#ifdef WIN32
    if (findHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(findHandle);
    }
#else
    closedir(dirp);
#endif
    /* End of the transfer */
    return rfbSendFileTransferMessage(cl, rfbDirPacket, 0, 0, 0, NULL);
}