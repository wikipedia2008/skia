/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/sksl/SkSLCompiler.h"

#include "tests/Test.h"

static void test_failure(skiatest::Reporter* r, const char* src, const char* error) {
    sk_sp<GrShaderCaps> caps = SkSL::ShaderCapsFactory::Default();
    SkSL::Compiler compiler(caps.get());
    SkSL::Program::Settings settings;
    std::unique_ptr<SkSL::Program> program = compiler.convertProgram(SkSL::Program::kFragment_Kind,
                                                                     SkSL::String(src), settings);
    if (program) {
        SkSL::String ignored;
        compiler.toSPIRV(*program, &ignored);
    }
    SkSL::String skError(error);
    if (compiler.errorText() != skError) {
        SkDebugf("SKSL ERROR:\n    source: %s\n    expected: %s    received: %s", src, error,
                 compiler.errorText().c_str());
    }
    REPORTER_ASSERT(r, compiler.errorText() == skError);
}

DEF_TEST(SkSLSPIRVBadOffset, r) {
    test_failure(r,
                 "struct Bad { layout (offset = 5) int x; } bad; void main() { bad.x = 5; "
                 "sk_FragColor.r = half(bad.x); }",
                 "error: 1: offset of field 'x' must be a multiple of 4\n1 error\n");
    test_failure(r,
                 "struct Bad { int x; layout (offset = 0) int y; } bad; void main() { bad.x = 5; "
                 "sk_FragColor.r = half(bad.x); }",
                 "error: 1: offset of field 'y' must be at least 4\n1 error\n");
}

DEF_TEST(SkSLSPIRVBadTypeInStruct, r) {
    test_failure(r,
                 "struct Bad { sampler x; }; uniform Bad b; void main() {}",
                 "error: 1: opaque type 'sampler' is not permitted in a struct\n1 error\n");
}

DEF_TEST(SkSLSPIRVBadTypeInInterfaceBlock, r) {
    test_failure(r,
                 "Bad { sampler x; }; void main() {}",
                 "error: 1: opaque type 'sampler' is not permitted in an interface block\n"
                 "1 error\n");
}
