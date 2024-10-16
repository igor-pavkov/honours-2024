static bool CanAlloc( size_t size )
{
    if( size < OGR_EXPAT_MAX_ALLOWED_ALLOC )
        return true;

    if( CPLTestBool(CPLGetConfigOption("OGR_EXPAT_UNLIMITED_MEM_ALLOC", "NO")) )
        return true;

    CPLError(CE_Failure, CPLE_OutOfMemory,
             "Expat tried to malloc %d bytes. File probably corrupted. "
             "This may also happen in case of a very big XML comment, in which case "
             "you may define the OGR_EXPAT_UNLIMITED_MEM_ALLOC configuration "
             "option to YES to remove that protection.",
             static_cast<int>(size));
    return false;
}