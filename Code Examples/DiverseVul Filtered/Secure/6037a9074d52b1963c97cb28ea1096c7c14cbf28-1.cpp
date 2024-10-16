rfbBool rfbSendFileTransferChunk(rfbClientPtr cl)
{
    /* Allocate buffer for compression */
    unsigned char readBuf[sz_rfbBlockSize];
    int bytesRead=0;
    int retval=0;
    fd_set wfds;
    struct timeval tv;
    int n;
#ifdef LIBVNCSERVER_HAVE_LIBZ
    unsigned char compBuf[sz_rfbBlockSize + 1024];
    unsigned long nMaxCompSize = sizeof(compBuf);
    int nRetC = 0;
#endif

    /*
     * Don't close the client if we get into this one because 
     * it is called from many places to service file transfers.
     * Note that permitFileTransfer is checked first.
     */
    if (cl->screen->permitFileTransfer != TRUE ||
       (cl->screen->getFileTransferPermission != NULL
        && cl->screen->getFileTransferPermission(cl) != TRUE)) { 
		return TRUE;
    }

    /* If not sending, or no file open...   Return as if we sent something! */
    if ((cl->fileTransfer.fd!=-1) && (cl->fileTransfer.sending==1))
    {
	FD_ZERO(&wfds);
        FD_SET(cl->sock, &wfds);

        /* return immediately */
	tv.tv_sec = 0; 
	tv.tv_usec = 0;
	n = select(cl->sock + 1, NULL, &wfds, NULL, &tv);

	if (n<0) {
#ifdef WIN32
	    errno=WSAGetLastError();
#endif
            rfbLog("rfbSendFileTransferChunk() select failed: %s\n", strerror(errno));
	}
        /* We have space on the transmit queue */
	if (n > 0)
	{
            bytesRead = read(cl->fileTransfer.fd, readBuf, sz_rfbBlockSize);
            switch (bytesRead) {
            case 0:
                /*
                rfbLog("rfbSendFileTransferChunk(): End-Of-File Encountered\n");
                */
                retval = rfbSendFileTransferMessage(cl, rfbEndOfFile, 0, 0, 0, NULL);
                close(cl->fileTransfer.fd);
                cl->fileTransfer.fd = -1;
                cl->fileTransfer.sending   = 0;
                cl->fileTransfer.receiving = 0;
                return retval;
            case -1:
                /* TODO : send an error msg to the client... */
#ifdef WIN32
	        errno=WSAGetLastError();
#endif
                rfbLog("rfbSendFileTransferChunk(): %s\n",strerror(errno));
                retval = rfbSendFileTransferMessage(cl, rfbAbortFileTransfer, 0, 0, 0, NULL);
                close(cl->fileTransfer.fd);
                cl->fileTransfer.fd = -1;
                cl->fileTransfer.sending   = 0;
                cl->fileTransfer.receiving = 0;
                return retval;
            default:
                /*
                rfbLog("rfbSendFileTransferChunk(): Read %d bytes\n", bytesRead);
                */
                if (!cl->fileTransfer.compressionEnabled)
                    return  rfbSendFileTransferMessage(cl, rfbFilePacket, 0, 0, bytesRead, (char *)readBuf);
                else
                {
#ifdef LIBVNCSERVER_HAVE_LIBZ
                    nRetC = compress(compBuf, &nMaxCompSize, readBuf, bytesRead);
                    /*
                    rfbLog("Compressed the packet from %d -> %d bytes\n", nMaxCompSize, bytesRead);
                    */
                    
                    if ((nRetC==0) && (nMaxCompSize<bytesRead))
                        return  rfbSendFileTransferMessage(cl, rfbFilePacket, 0, 1, nMaxCompSize, (char *)compBuf);
                    else
                        return  rfbSendFileTransferMessage(cl, rfbFilePacket, 0, 0, bytesRead, (char *)readBuf);
#else
                    /* We do not support compression of the data stream */
                    return  rfbSendFileTransferMessage(cl, rfbFilePacket, 0, 0, bytesRead, (char *)readBuf);
#endif
                }
            }
        }
    }
    return TRUE;
}