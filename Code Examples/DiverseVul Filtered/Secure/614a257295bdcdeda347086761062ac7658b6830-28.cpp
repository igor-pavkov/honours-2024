MagickExport MagickBooleanType OpenBlob(const ImageInfo *image_info,
  Image *image,const BlobMode mode,ExceptionInfo *exception)
{
  BlobInfo
    *magick_restrict blob_info;

  char
    extension[MaxTextExtent],
    filename[MaxTextExtent];

  const char
    *type;

  MagickBooleanType
    status;

  PolicyRights
    rights;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  blob_info=image->blob;
  if (image_info->blob != (void *) NULL)
    {
      if (image_info->stream != (StreamHandler) NULL)
        blob_info->stream=(StreamHandler) image_info->stream;
      AttachBlob(blob_info,image_info->blob,image_info->length);
      return(MagickTrue);
    }
  (void) DetachBlob(blob_info);
  blob_info->mode=mode;
  switch (mode)
  {
    default: type="r"; break;
    case ReadBlobMode: type="r"; break;
    case ReadBinaryBlobMode: type="rb"; break;
    case WriteBlobMode: type="w"; break;
    case WriteBinaryBlobMode: type="w+b"; break;
    case AppendBlobMode: type="a"; break;
    case AppendBinaryBlobMode: type="a+b"; break;
  }
  if (*type != 'r')
    blob_info->synchronize=image_info->synchronize;
  if (image_info->stream != (StreamHandler) NULL)
    {
      blob_info->stream=(StreamHandler) image_info->stream;
      if (*type == 'w')
        {
          blob_info->type=FifoStream;
          return(MagickTrue);
        }
    }
  /*
    Open image file.
  */
  *filename='\0';
  (void) CopyMagickString(filename,image->filename,MaxTextExtent);
  rights=ReadPolicyRights;
  if (*type == 'w')
    rights=WritePolicyRights;
  if (IsRightsAuthorized(PathPolicyDomain,rights,filename) == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",filename);
      return(MagickFalse);
    }
  if ((LocaleCompare(filename,"-") == 0) ||
      ((*filename == '\0') && (image_info->file == (FILE *) NULL)))
    {
      blob_info->file_info.file=(*type == 'r') ? stdin : stdout;
#if defined(MAGICKCORE_WINDOWS_SUPPORT) || defined(__OS2__)
      if (strchr(type,'b') != (char *) NULL)
        setmode(fileno(blob_info->file_info.file),_O_BINARY);
#endif
      blob_info->type=StandardStream;
      blob_info->exempt=MagickTrue;
      return(SetStreamBuffering(image_info,image));
    }
  if ((LocaleNCompare(filename,"fd:",3) == 0) &&
      (IsGeometry(filename+3) != MagickFalse))
    {
      char
        mode[MaxTextExtent];

      *mode=(*type);
      mode[1]='\0';
      blob_info->file_info.file=fdopen(StringToLong(filename+3),mode);
      if (blob_info->file_info.file == (FILE *) NULL)
        {
          ThrowFileException(exception,BlobError,"UnableToOpenBlob",filename);
          return(MagickFalse);
        }
#if defined(MAGICKCORE_WINDOWS_SUPPORT) || defined(__OS2__)
      if (strchr(type,'b') != (char *) NULL)
        setmode(fileno(blob_info->file_info.file),_O_BINARY);
#endif
      blob_info->type=FileStream;
      blob_info->exempt=MagickTrue;
      return(SetStreamBuffering(image_info,image));
    }
#if defined(MAGICKCORE_HAVE_POPEN) && defined(MAGICKCORE_PIPES_SUPPORT)
  if (*filename == '|')
    {
      char
        fileMode[MaxTextExtent],
        *sanitize_command;

      /*
        Pipe image to or from a system command.
      */
#if defined(SIGPIPE)
      if (*type == 'w')
        (void) signal(SIGPIPE,SIG_IGN);
#endif
      *fileMode=(*type);
      fileMode[1]='\0';
      sanitize_command=SanitizeString(filename+1);
      blob_info->file_info.file=(FILE *) popen_utf8(sanitize_command,
        fileMode);
      sanitize_command=DestroyString(sanitize_command);
      if (blob_info->file_info.file == (FILE *) NULL)
        {
          ThrowFileException(exception,BlobError,"UnableToOpenBlob",filename);
          return(MagickFalse);
        }
      blob_info->type=PipeStream;
      blob_info->exempt=MagickTrue;
      return(SetStreamBuffering(image_info,image));
    }
#endif
  status=GetPathAttributes(filename,&blob_info->properties);
#if defined(S_ISFIFO)
  if ((status != MagickFalse) && S_ISFIFO(blob_info->properties.st_mode))
    {
      blob_info->file_info.file=(FILE *) fopen_utf8(filename,type);
      if (blob_info->file_info.file == (FILE *) NULL)
        {
          ThrowFileException(exception,BlobError,"UnableToOpenBlob",filename);
          return(MagickFalse);
        }
      blob_info->type=FileStream;
      blob_info->exempt=MagickTrue;
      return(SetStreamBuffering(image_info,image));
    }
#endif
  GetPathComponent(image->filename,ExtensionPath,extension);
  if (*type == 'w')
    {
      (void) CopyMagickString(filename,image->filename,MaxTextExtent);
      if ((image_info->adjoin == MagickFalse) ||
          (strchr(filename,'%') != (char *) NULL))
        {
          /*
            Form filename for multi-part images.
          */
          (void) InterpretImageFilename(image_info,image,image->filename,(int)
            image->scene,filename);
          if ((LocaleCompare(filename,image->filename) == 0) &&
              ((GetPreviousImageInList(image) != (Image *) NULL) ||
               (GetNextImageInList(image) != (Image *) NULL)))
            {
              char
                path[MaxTextExtent];

              GetPathComponent(image->filename,RootPath,path);
              if (*extension == '\0')
                (void) FormatLocaleString(filename,MaxTextExtent,"%s-%.20g",
                  path,(double) image->scene);
              else
                (void) FormatLocaleString(filename,MaxTextExtent,"%s-%.20g.%s",
                  path,(double) image->scene,extension);
            }
          (void) CopyMagickString(image->filename,filename,MaxTextExtent);
#if defined(macintosh)
          SetApplicationType(filename,image_info->magick,'8BIM');
#endif
        }
    }
  if (image_info->file != (FILE *) NULL)
    {
      blob_info->file_info.file=image_info->file;
      blob_info->type=FileStream;
      blob_info->exempt=MagickTrue;
    }
  else
    if (*type == 'r')
      {
        blob_info->file_info.file=(FILE *) fopen_utf8(filename,type);
        if (blob_info->file_info.file != (FILE *) NULL)
          {
            size_t
              count;

            unsigned char
              magick[3];

            blob_info->type=FileStream;
            (void) fstat(fileno(blob_info->file_info.file),
              &blob_info->properties);
            (void) SetStreamBuffering(image_info,image);
            (void) memset(magick,0,sizeof(magick));
            count=fread(magick,1,sizeof(magick),blob_info->file_info.file);
            (void) fseek(blob_info->file_info.file,-((off_t) count),SEEK_CUR);
#if defined(MAGICKCORE_POSIX_SUPPORT)
            (void) fflush(blob_info->file_info.file);
#endif
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
               "  read %.20g magic header bytes",(double) count);
#if defined(MAGICKCORE_ZLIB_DELEGATE)
            if (((int) magick[0] == 0x1F) && ((int) magick[1] == 0x8B) &&
                ((int) magick[2] == 0x08))
              {
                if (blob_info->file_info.file != (FILE *) NULL)
                  (void) fclose(blob_info->file_info.file);
                blob_info->file_info.file=(FILE *) NULL;
                blob_info->file_info.gzfile=gzopen(filename,"rb");
                if (blob_info->file_info.gzfile != (gzFile) NULL)
                  blob_info->type=ZipStream;
               }
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
            if (strncmp((char *) magick,"BZh",3) == 0)
              {
                if (blob_info->file_info.file != (FILE *) NULL)
                  (void) fclose(blob_info->file_info.file);
                blob_info->file_info.file=(FILE *) NULL;
                blob_info->file_info.bzfile=BZ2_bzopen(filename,"r");
                if (blob_info->file_info.bzfile != (BZFILE *) NULL)
                  blob_info->type=BZipStream;
              }
#endif
            if (blob_info->type == FileStream)
              {
                const MagickInfo
                  *magick_info;

                ExceptionInfo
                  *sans_exception;

                size_t
                  length;

                sans_exception=AcquireExceptionInfo();
                magick_info=GetMagickInfo(image_info->magick,sans_exception);
                sans_exception=DestroyExceptionInfo(sans_exception);
                length=(size_t) blob_info->properties.st_size;
                if ((magick_info != (const MagickInfo *) NULL) &&
                    (GetMagickBlobSupport(magick_info) != MagickFalse) &&
                    (length > MagickMaxBufferExtent) &&
                    (AcquireMagickResource(MapResource,length) != MagickFalse))
                  {
                    void
                      *blob;

                    blob=MapBlob(fileno(blob_info->file_info.file),ReadMode,0,
                      length);
                    if (blob == (void *) NULL)
                      RelinquishMagickResource(MapResource,length);
                    else
                      {
                        /*
                          Format supports blobs-- use memory-mapped I/O.
                        */
                        if (image_info->file != (FILE *) NULL)
                          blob_info->exempt=MagickFalse;
                        else
                          {
                            (void) fclose(blob_info->file_info.file);
                            blob_info->file_info.file=(FILE *) NULL;
                          }
                        AttachBlob(blob_info,blob,length);
                        blob_info->mapped=MagickTrue;
                      }
                  }
              }
          }
        }
      else
#if defined(MAGICKCORE_ZLIB_DELEGATE)
        if ((LocaleCompare(extension,"Z") == 0) ||
            (LocaleCompare(extension,"gz") == 0) ||
            (LocaleCompare(extension,"wmz") == 0) ||
            (LocaleCompare(extension,"svgz") == 0))
          {
            blob_info->file_info.gzfile=gzopen(filename,"wb");
            if (blob_info->file_info.gzfile != (gzFile) NULL)
              blob_info->type=ZipStream;
          }
        else
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
          if (LocaleCompare(extension,"bz2") == 0)
            {
              if (mode == WriteBinaryBlobMode)
                type="w";
              blob_info->file_info.bzfile=BZ2_bzopen(filename,"w");
              if (blob_info->file_info.bzfile != (BZFILE *) NULL)
                blob_info->type=BZipStream;
            }
          else
#endif
            {
              blob_info->file_info.file=(FILE *) fopen_utf8(filename,type);
              if (blob_info->file_info.file != (FILE *) NULL)
                {
                  blob_info->type=FileStream;
                  (void) SetStreamBuffering(image_info,image);
                }
       }
  blob_info->status=MagickFalse;
  if (blob_info->type != UndefinedStream)
    blob_info->size=GetBlobSize(image);
  else
    {
      ThrowFileException(exception,BlobError,"UnableToOpenBlob",filename);
      return(MagickFalse);
    }
  return(MagickTrue);
}