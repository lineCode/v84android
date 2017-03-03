// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libplatform/libplatform.h"
#include "v8.h"

using namespace v8;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void *Allocate(size_t length)
    {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void *AllocateUninitialized(size_t length)
    {
        return malloc(length);
    }

    virtual void Free(void *data, size_t)
    {
        free(data);
    }
};

static bool ReadAll(const char *filename, char **pBuf)
{
    FILE *file = fopen(filename, "r");
    if (file) {
        size_t size = 1024;
        size_t count = 0;
        char *buf = (char *) malloc(size);

        while (!feof(file) && !ferror(file)) {
            count += fread(buf, 1, size - count - 1, file);

            if (count == size - 1) {
                size <<= 1;
                char *newBuf = (char *) realloc(buf, size);
                if (newBuf == NULL) {
                    fclose(file);
                    free(buf);
                    return false;
                } else {
                    buf = newBuf;
                }
            }
        }

        if (feof(file)) {
            fclose(file);
            buf[count] = 0;
            if (pBuf) {
                *pBuf = buf;
            }
            return true;
        } else {
            fclose(file);
            free(buf);
        }
    }
    return false;
}


int main(int argc, char *argv[])
{
    // Initialize V8.
    V8::InitializeExternalStartupData(argv[0]);
    Platform *platform = platform::CreateDefaultPlatform();
    V8::InitializePlatform(platform);
    V8::Initialize();
    // Create a new Isolate and make it the current one.
    ArrayBufferAllocator allocator;
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = &allocator;
    Isolate *isolate = Isolate::New(create_params);
    {
        Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);
        // Create a new context.
        Local<Context> context = Context::New(isolate);
        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);
        // Create a string containing the JavaScript source code.
        Local<String> source;
        char *buf = nullptr;
        if (argc > 1 && ReadAll(argv[1], &buf)) {
            source = String::NewFromUtf8(isolate, buf, NewStringType::kNormal).ToLocalChecked();
            free(buf);
        } else {
            source = String::NewFromUtf8(isolate, "'Hello' + ', World!'",
                                         NewStringType::kNormal).ToLocalChecked();
        }

        TryCatch trycatch(isolate);
        // Compile the source code.
        Local<Script> script = Script::Compile(context, source).ToLocalChecked();
        // Run the script to get the result.
        MaybeLocal<Value> maybeLocal = script->Run(context);
        if (maybeLocal.IsEmpty()) {
            Local<Value> except = trycatch.Exception();
            String::Utf8Value exception_str(except);
            printf("Exception: %s\n", *exception_str);
            puts(">>>>>>>>>>>>>>>>>\n");
        } else {
            Local<Value> result = maybeLocal.ToLocalChecked();
            // Convert the result to an UTF8 string and print it.
            String::Utf8Value utf8(result);
            printf("%s\n", *utf8);
        }
    }
    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete platform;
    return 0;
}