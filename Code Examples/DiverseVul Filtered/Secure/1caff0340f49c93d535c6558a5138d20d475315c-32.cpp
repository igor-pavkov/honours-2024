static void e1000_instance_init(Object *obj)
{
    E1000State *n = E1000(obj);
    device_add_bootindex_property(obj, &n->conf.bootindex,
                                  "bootindex", "/ethernet-phy@0",
                                  DEVICE(n));
}