void dofeat(void)
{
# define FEAT  "Extensions supported:" CRLF \
    " UTF8" CRLF \
    " EPRT" CRLF " IDLE" CRLF " MDTM" CRLF " SIZE" CRLF " MFMT" CRLF \
    " REST STREAM" CRLF \
    " MLST type*;size*;sizd*;modify*;UNIX.mode*;UNIX.uid*;UNIX.gid*;unique*;" CRLF \
    " MLSD" CRLF \
    " PRET"

# ifdef WITH_TLS
#  define FEAT_TLS CRLF " AUTH TLS" CRLF " PBSZ" CRLF " PROT"
# else
#  define FEAT_TLS ""
# endif
# ifdef DEBUG
#  define FEAT_DEBUG CRLF " XDBG"
# else
#  define FEAT_DEBUG ""
# endif
# ifdef WITH_VIRTUAL_CHROOT
#  define FEAT_TVFS ""
# else
#  define FEAT_TVFS CRLF " TVFS"
# endif
# define FEAT_PASV CRLF " PASV" CRLF " EPSV"

# ifdef MINIMAL
#  define FEAT_ESTA ""
#  define FEAT_ESTP ""
# else
#  define FEAT_ESTA CRLF " ESTA"
#  define FEAT_ESTP CRLF " ESTP"
# endif

    char feat[] = FEAT FEAT_DEBUG FEAT_TLS FEAT_TVFS FEAT_ESTA FEAT_PASV FEAT_ESTP;

    if (disallow_passive != 0) {
        feat[sizeof FEAT FEAT_DEBUG FEAT_TLS FEAT_TVFS FEAT_ESTA - 1U] = 0;
    }
# ifndef MINIMAL
    else if (STORAGE_FAMILY(force_passive_ip) != 0) {
        feat[sizeof FEAT FEAT_DEBUG FEAT_TLS FEAT_TVFS FEAT_ESTA FEAT_PASV - 1U] = 0;
    }
# endif
    addreply_noformat(0, feat);
    addreply_noformat(211, "End.");
}