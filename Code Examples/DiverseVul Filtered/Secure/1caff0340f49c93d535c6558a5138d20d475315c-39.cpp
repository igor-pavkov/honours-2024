static void e1000_register_types(void)
{
    int i;

    type_register_static(&e1000_base_info);
    for (i = 0; i < ARRAY_SIZE(e1000_devices); i++) {
        const E1000Info *info = &e1000_devices[i];
        TypeInfo type_info = {};

        type_info.name = info->name;
        type_info.parent = TYPE_E1000_BASE;
        type_info.class_data = (void *)info;
        type_info.class_init = e1000_class_init;

        type_register(&type_info);
    }
}