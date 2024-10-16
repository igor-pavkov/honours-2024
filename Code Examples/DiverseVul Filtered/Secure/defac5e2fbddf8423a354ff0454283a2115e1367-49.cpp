static FDriveSize drive_size(FloppyDriveType drive)
{
    switch (drive) {
    case FLOPPY_DRIVE_TYPE_120:
        return FDRIVE_SIZE_525;
    case FLOPPY_DRIVE_TYPE_144:
    case FLOPPY_DRIVE_TYPE_288:
        return FDRIVE_SIZE_350;
    default:
        return FDRIVE_SIZE_UNKNOWN;
    }
}