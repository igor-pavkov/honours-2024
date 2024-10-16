static void fdc_register_types(void)
{
    type_register_static(&floppy_bus_info);
    type_register_static(&floppy_drive_info);
}