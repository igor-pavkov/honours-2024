  inline CapTableBuilder* getLocalCapTable() {
    // Return a CapTableBuilder that merely implements local loopback. That is, you can set
    // capabilities, then read the same capabilities back, but there is no intent ever to transmit
    // these capabilities. A MessageBuilder that isn't imbued with some other CapTable uses this
    // by default.
    //
    // TODO(cleanup): It's sort of a hack that this exists. In theory, perhaps, unimbued
    //   MessageBuilders should throw exceptions on any attempt to access capability fields, like
    //   unimbued MessageReaders do. However, lots of code exists which uses MallocMessageBuilder
    //   as a temporary holder for data to be copied in and out (without being serialized), and it
    //   is expected that such data can include capabilities, which is admittedly reasonable.
    //   Therefore, all MessageBuilders must have a cap table by default. Arguably we should
    //   deprecate this usage and instead define a new helper type for this exact purpose.

    return &localCapTable;
  }