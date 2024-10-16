void createHermesBuiltins(
    Runtime *runtime,
    llvm::MutableArrayRef<NativeFunction *> builtins) {
  auto defineInternMethod = [&](BuiltinMethod::Enum builtinIndex,
                                Predefined::Str symID,
                                NativeFunctionPtr func,
                                uint8_t count = 0) {
    auto method = NativeFunction::create(
        runtime,
        Handle<JSObject>::vmcast(&runtime->functionPrototype),
        nullptr /* context */,
        func,
        Predefined::getSymbolID(symID),
        count,
        Runtime::makeNullHandle<JSObject>());

    assert(builtins[builtinIndex] == nullptr && "builtin already defined");
    builtins[builtinIndex] = *method;
  };

  // HermesBuiltin function properties
  namespace P = Predefined;
  namespace B = BuiltinMethod;
  defineInternMethod(
      B::HermesBuiltin_silentSetPrototypeOf,
      P::silentSetPrototypeOf,
      silentObjectSetPrototypeOf,
      2);
  defineInternMethod(
      B::HermesBuiltin_getTemplateObject,
      P::getTemplateObject,
      hermesBuiltinGetTemplateObject);
  defineInternMethod(
      B::HermesBuiltin_ensureObject,
      P::ensureObject,
      hermesBuiltinEnsureObject,
      2);
  defineInternMethod(
      B::HermesBuiltin_throwTypeError,
      P::throwTypeError,
      hermesBuiltinThrowTypeError,
      1);
  defineInternMethod(
      B::HermesBuiltin_generatorSetDelegated,
      P::generatorSetDelegated,
      hermesBuiltinGeneratorSetDelegated,
      1);
  defineInternMethod(
      B::HermesBuiltin_copyDataProperties,
      P::copyDataProperties,
      hermesBuiltinCopyDataProperties,
      3);
  defineInternMethod(
      B::HermesBuiltin_copyRestArgs,
      P::copyRestArgs,
      hermesBuiltinCopyRestArgs,
      1);
  defineInternMethod(
      B::HermesBuiltin_arraySpread,
      P::arraySpread,
      hermesBuiltinArraySpread,
      2);
  defineInternMethod(B::HermesBuiltin_apply, P::apply, hermesBuiltinApply, 2);
  defineInternMethod(
      B::HermesBuiltin_exportAll, P::exportAll, hermesBuiltinExportAll);
  defineInternMethod(
      B::HermesBuiltin_exponentiationOperator,
      P::exponentiationOperator,
      mathPow);

  // Define the 'requireFast' function, which takes a number argument.
  defineInternMethod(
      B::HermesBuiltin_requireFast, P::requireFast, requireFast, 1);
}