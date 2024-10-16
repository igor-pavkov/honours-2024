CallResult<bool> JSObject::defineOwnComputedPrimitive(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    DefinePropertyFlags dpFlags,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  assert(
      !nameValHandle->isObject() &&
      "nameValHandle passed to "
      "defineOwnComputedPrimitive() cannot be "
      "an object");
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
  assert(
      !dpFlags.enableInternalSetter &&
      "Cannot set internalSetter on a computed property");
#ifndef NDEBUG
  if (dpFlags.isAccessor()) {
    assert(valueOrAccessor.get().isPointer() && "accessor must be non-empty");
    assert(
        !dpFlags.setWritable && !dpFlags.writable &&
        "writable must not be set with accessors");
  }
#endif

  // If the name is a valid integer array index, store it here.
  OptValue<uint32_t> arrayIndex;

  // If we have indexed storage, we must attempt to convert the name to array
  // index, even if the conversion is expensive.
  if (selfHandle->flags_.indexedStorage) {
    MutableHandle<StringPrimitive> strPrim{runtime};
    TO_ARRAY_INDEX(runtime, nameValHandle, strPrim, arrayIndex);
  }

  SymbolID id{};

  // If not storing a property with an array index name, or if we don't have
  // indexed storage, just pass to the named routine.
  if (!arrayIndex) {
    LAZY_TO_IDENTIFIER(runtime, nameValHandle, id);
    return defineOwnPropertyInternal(
        selfHandle, runtime, id, dpFlags, valueOrAccessor, opFlags);
  }

  // At this point we know that we have indexed storage and that the property
  // has an index-like name.

  // First check if a named property with the same name exists.
  if (selfHandle->clazz_.get(runtime)->getHasIndexLikeProperties()) {
    LAZY_TO_IDENTIFIER(runtime, nameValHandle, id);

    NamedPropertyDescriptor desc;
    auto pos = findProperty(selfHandle, runtime, id, desc);
    // If we found a named property, update it.
    if (pos) {
      return updateOwnProperty(
          selfHandle,
          runtime,
          id,
          *pos,
          desc,
          dpFlags,
          valueOrAccessor,
          opFlags);
    }
  }

  // Does an indexed property with that index exist?
  auto indexedPropPresent =
      getOwnIndexedPropertyFlags(selfHandle.get(), runtime, *arrayIndex);
  if (indexedPropPresent) {
    // The current value of the property.
    HermesValue curValueOrAccessor =
        getOwnIndexed(selfHandle.get(), runtime, *arrayIndex);

    auto updateStatus = checkPropertyUpdate(
        runtime,
        *indexedPropPresent,
        dpFlags,
        curValueOrAccessor,
        valueOrAccessor,
        opFlags);
    if (updateStatus == ExecutionStatus::EXCEPTION)
      return ExecutionStatus::EXCEPTION;
    if (updateStatus->first == PropertyUpdateStatus::failed)
      return false;

    // The property update is valid, but can the property remain an "indexed"
    // property, or do we need to convert it to a named property?
    // If the property flags didn't change, the property remains indexed.
    if (updateStatus->second == *indexedPropPresent) {
      // If the value doesn't change, we are done.
      if (updateStatus->first == PropertyUpdateStatus::done)
        return true;

      // If we successfully updated the value, we are done.
      auto result =
          setOwnIndexed(selfHandle, runtime, *arrayIndex, valueOrAccessor);
      if (LLVM_UNLIKELY(result == ExecutionStatus::EXCEPTION))
        return ExecutionStatus::EXCEPTION;
      if (*result)
        return true;

      if (opFlags.getThrowOnError()) {
        // TODO: better error message.
        return runtime->raiseTypeError(
            "cannot change read-only property value");
      }

      return false;
    }

    // OK, we need to convert an indexed property to a named one.

    // Check whether to use the supplied value, or to reuse the old one, as we
    // are simply reconfiguring it.
    MutableHandle<> value{runtime};
    if (dpFlags.setValue || dpFlags.isAccessor()) {
      value = valueOrAccessor.get();
    } else {
      value = curValueOrAccessor;
    }

    // Update dpFlags to match the existing property flags.
    dpFlags.setEnumerable = 1;
    dpFlags.setWritable = 1;
    dpFlags.setConfigurable = 1;
    dpFlags.enumerable = updateStatus->second.enumerable;
    dpFlags.writable = updateStatus->second.writable;
    dpFlags.configurable = updateStatus->second.configurable;

    // Delete the existing indexed property.
    if (!deleteOwnIndexed(selfHandle, runtime, *arrayIndex)) {
      if (opFlags.getThrowOnError()) {
        // TODO: better error message.
        return runtime->raiseTypeError("Cannot define property");
      }
      return false;
    }

    // Add the new named property.
    LAZY_TO_IDENTIFIER(runtime, nameValHandle, id);
    return addOwnProperty(selfHandle, runtime, id, dpFlags, value, opFlags);
  }

  /// Can we add new properties?
  if (!selfHandle->isExtensible()) {
    if (opFlags.getThrowOnError()) {
      return runtime->raiseTypeError(
          "cannot add a new property"); // TODO: better message.
    }
    return false;
  }

  // This is a new property with an index-like name.
  // Check whether we need to update array's ".length" property.
  bool updateLength = false;
  if (auto arrayHandle = Handle<JSArray>::dyn_vmcast(selfHandle)) {
    if (LLVM_UNLIKELY(*arrayIndex >= JSArray::getLength(*arrayHandle))) {
      NamedPropertyDescriptor lengthDesc;
      bool lengthPresent = getOwnNamedDescriptor(
          arrayHandle,
          runtime,
          Predefined::getSymbolID(Predefined::length),
          lengthDesc);
      (void)lengthPresent;
      assert(lengthPresent && ".length must be present in JSArray");

      if (!lengthDesc.flags.writable) {
        if (opFlags.getThrowOnError()) {
          return runtime->raiseTypeError(
              "Cannot assign to read-only 'length' property of array");
        }
        return false;
      }

      updateLength = true;
    }
  }

  bool newIsIndexed = canNewPropertyBeIndexed(dpFlags);
  if (newIsIndexed) {
    auto result = setOwnIndexed(
        selfHandle,
        runtime,
        *arrayIndex,
        dpFlags.setValue ? valueOrAccessor : Runtime::getUndefinedValue());
    if (LLVM_UNLIKELY(result == ExecutionStatus::EXCEPTION))
      return ExecutionStatus::EXCEPTION;
    if (!*result) {
      if (opFlags.getThrowOnError()) {
        // TODO: better error message.
        return runtime->raiseTypeError("Cannot define property");
      }
      return false;
    }
  }

  // If this is an array and we need to update ".length", do so.
  if (updateLength) {
    // This should always succeed since we are simply enlarging the length.
    auto res = JSArray::setLength(
        Handle<JSArray>::vmcast(selfHandle), runtime, *arrayIndex + 1, opFlags);
    (void)res;
    assert(
        res != ExecutionStatus::EXCEPTION && *res &&
        "JSArray::setLength() failed unexpectedly");
  }

  if (newIsIndexed)
    return true;

  // We are adding a new property with an index-like name.
  LAZY_TO_IDENTIFIER(runtime, nameValHandle, id);
  return addOwnProperty(
      selfHandle, runtime, id, dpFlags, valueOrAccessor, opFlags);
}