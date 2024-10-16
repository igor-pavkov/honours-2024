void Init(v8::Local<v8::Object> exports) {
	exports->Set(Nan::New<v8::String>("detectCharacterEncoding").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(DetectCharacterEncoding)->GetFunction());
}