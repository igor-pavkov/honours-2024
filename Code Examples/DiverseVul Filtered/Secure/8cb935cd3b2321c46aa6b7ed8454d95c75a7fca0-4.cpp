CallResult<bool> JSObject::defineOwnPropertyInternal(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    DefinePropertyFlags dpFlags,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  assert(
      !opFlags.getMustExist() && "cannot use mustExist with defineOwnProperty");
  assert(
      !(dpFlags.setValue && dpFlags.isAccessor()) &&
      "Cannot set both value and accessor");
  assert(
      (dpFlags.setValue || dpFlags.isAccessor() ||
       valueOrAccessor.get().isUndefined()) &&
      "value must be undefined when all of setValue/setSetter/setGetter are "
      "false");
#ifndef NDEBUG
  if (dpFlags.isAccessor()) {
    assert(valueOrAccessor.get().isPointer() && "accessor must be non-empty");
    assert(
        !dpFlags.setWritable && !dpFlags.writable &&
        "writable must not be set with accessors");
  }
#endif

  // Is it an existing property.
  NamedPropertyDescriptor desc;
  auto pos = findProperty(selfHandle, runtime, name, desc);
  if (pos) {
    return updateOwnProperty(
        selfHandle,
        runtime,
        name,
        *pos,
        desc,
        dpFlags,
        valueOrAccessor,
        opFlags);
  }

  if (LLVM_UNLIKELY(
          selfHandle->flags_.lazyObject || selfHandle->flags_.proxyObject)) {
    if (selfHandle->flags_.proxyObject) {
      return JSProxy::defineOwnProperty(
          selfHandle,
          runtime,
          name.isUniqued() ? runtime->makeHandle(HermesValue::encodeStringValue(
                                 runtime->getStringPrimFromSymbolID(name)))
                           : runtime->makeHandle(name),
          dpFlags,
          valueOrAccessor,
          opFlags);
    }
    assert(selfHandle->flags_.lazyObject && "descriptor flags are impossible");
    // if the property was not found and the object is lazy we need to
    // initialize it and try again.
    JSObject::initializeLazyObject(runtime, selfHandle);
    return defineOwnPropertyInternal(
        selfHandle, runtime, name, dpFlags, valueOrAccessor, opFlags);
  }

  return addOwnProperty(
      selfHandle, runtime, name, dpFlags, valueOrAccessor, opFlags);
}