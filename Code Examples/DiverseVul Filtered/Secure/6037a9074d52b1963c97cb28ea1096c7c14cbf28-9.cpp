rfbBool rfbSendDirContent(rfbClientPtr cl, int length, char *buffer)
{
    char retfilename[MAX_PATH];
    char path[MAX_PATH];
    struct stat statbuf;
    RFB_FIND_DATA win32filename;
    int nOptLen = 0, retval=0;
    DIR *dirp=NULL;
    struct dirent *direntp=NULL;

    FILEXFER_ALLOWED_OR_CLOSE_AND_RETURN("", cl, FALSE);

    /* Client thinks we are Winblows */
    rfbFilenameTranslate2UNIX(cl, buffer, path);

    if (DB) rfbLog("rfbProcessFileTransfer() rfbDirContentRequest: rfbRDirContent: \"%s\"->\"%s\"\n",buffer, path);

    dirp=opendir(path);
    if (dirp==NULL)
        return rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, 0, NULL);
    /* send back the path name (necessary for links) */
    if (rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, length, buffer)==FALSE) return FALSE;
    for (direntp=readdir(dirp); direntp!=NULL; direntp=readdir(dirp))
    {
        /* get stats */
        snprintf(retfilename,sizeof(retfilename),"%s/%s", path, direntp->d_name);
        retval = stat(retfilename, &statbuf);

        if (retval==0)
        {
            memset((char *)&win32filename, 0, sizeof(win32filename));
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
            
            /* Do not show hidden files (but show how to move up the tree) */
            if ((strcmp(direntp->d_name, "..")==0) || (direntp->d_name[0]!='.'))
            {
                nOptLen = sizeof(RFB_FIND_DATA) - MAX_PATH - 14 + strlen((char *)win32filename.cFileName);
                /*
                rfbLog("rfbProcessFileTransfer() rfbDirContentRequest: rfbRDirContent: Sending \"%s\"\n", (char *)win32filename.cFileName);
                */
                if (rfbSendFileTransferMessage(cl, rfbDirPacket, rfbADirectory, 0, nOptLen, (char *)&win32filename)==FALSE)
                {
                    closedir(dirp);
                    return FALSE;
                }
            }
        }
    }
    closedir(dirp);
    /* End of the transfer */
    return rfbSendFileTransferMessage(cl, rfbDirPacket, 0, 0, 0, NULL);
}