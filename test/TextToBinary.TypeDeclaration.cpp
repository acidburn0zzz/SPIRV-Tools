// Copyright (c) 2015 The Khronos Group Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
// KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
// SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
//    https://www.khronos.org/registry/
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

// Assembler tests for instructions in the "Type-Declaration" section of the
// SPIR-V spec.

#include "UnitSPIRV.h"

#include "gmock/gmock.h"
#include "TestFixture.h"

namespace {

using spvtest::EnumCase;
using spvtest::MakeInstruction;
using ::testing::Eq;

// Test Dim enums via OpTypeImage

using DimTest =
    spvtest::TextToBinaryTestBase<::testing::TestWithParam<EnumCase<spv::Dim>>>;

TEST_P(DimTest, AnyDim) {
  std::string input = "%imageType = OpTypeImage %sampledType " +
                      GetParam().name() + " 2 3 0 4 Rgba8";
  EXPECT_THAT(
      CompiledInstructions(input),
      Eq(MakeInstruction(spv::OpTypeImage, {1, 2, GetParam().value(), 2, 3, 0,
                                            4, spv::ImageFormatRgba8})));
}

// clang-format off
#define CASE(NAME) {spv::Dim##NAME, #NAME}
INSTANTIATE_TEST_CASE_P(
    TextToBinaryDim, DimTest,
    ::testing::ValuesIn(std::vector<EnumCase<spv::Dim>>{
        CASE(1D),
        CASE(2D),
        CASE(3D),
        CASE(Cube),
        CASE(Rect),
        CASE(Buffer),
        CASE(InputTarget),
    }));
#undef CASE
// clang-format on

// Test ImageFormat enums via OpTypeImage

using ImageFormatTest = spvtest::TextToBinaryTestBase<
    ::testing::TestWithParam<EnumCase<spv::ImageFormat>>>;

TEST_P(ImageFormatTest, AnyImageFormat) {
  std::string input =
      "%imageType = OpTypeImage %sampledType 1D  2 3 0 4 " + GetParam().name();
  EXPECT_THAT(CompiledInstructions(input),
              Eq(MakeInstruction(spv::OpTypeImage, {1, 2, spv::Dim1D, 2, 3, 0,
                                                    4, GetParam().value()})));
}

// clang-format off
#define CASE(NAME) {spv::ImageFormat##NAME, #NAME}
INSTANTIATE_TEST_CASE_P(
    TextToBinaryImageFormat, ImageFormatTest,
    ::testing::ValuesIn(std::vector<EnumCase<spv::ImageFormat>>{
        CASE(Unknown),
        CASE(Rgba32f),
        CASE(Rgba16f),
        CASE(R32f),
        CASE(Rgba8),
        CASE(Rgba8Snorm),
        CASE(Rg32f),
        CASE(Rg16f),
        CASE(R11fG11fB10f),
        CASE(R16f),
        CASE(Rgba16),
        CASE(Rgb10A2),
        CASE(Rg16),
        CASE(Rg8),
        CASE(R16),
        CASE(R8),
        CASE(Rgba16Snorm),
        CASE(Rg16Snorm),
        CASE(Rg8Snorm),
        CASE(R16Snorm),
        CASE(R8Snorm),
        CASE(Rgba32i),
        CASE(Rgba16i),
        CASE(Rgba8i),
        CASE(R32i),
        CASE(Rg32i),
        CASE(Rg16i),
        CASE(Rg8i),
        CASE(R16i),
        CASE(R8i),
        CASE(Rgba32ui),
        CASE(Rgba16ui),
        CASE(Rgba8ui),
        CASE(R32ui),
        CASE(Rgb10a2ui),
        CASE(Rg32ui),
        CASE(Rg16ui),
        CASE(Rg8ui),
        CASE(R16ui),
        CASE(R8ui),
    }));
#undef CASE
// clang-format on

// Test AccessQualifier enums via OpTypePipe.

using OpTypePipeTest = spvtest::TextToBinaryTestBase<
    ::testing::TestWithParam<EnumCase<spv::AccessQualifier>>>;

TEST_P(OpTypePipeTest, AnyAccessQualifier) {
  std::string input = "%1 = OpTypePipe " + GetParam().name();
  EXPECT_THAT(CompiledInstructions(input),
              Eq(MakeInstruction(spv::OpTypePipe, {1, GetParam().value()})));
}

// clang-format off
#define CASE(NAME) {spv::AccessQualifier##NAME, #NAME}
INSTANTIATE_TEST_CASE_P(
    TextToBinaryTypePipe, OpTypePipeTest,
    ::testing::ValuesIn(std::vector<EnumCase<spv::AccessQualifier>>{
                            CASE(ReadOnly),
                            CASE(WriteOnly),
                            CASE(ReadWrite),
                        }));
#undef CASE
// clang-format on

using OpTypeForwardPointerTest = spvtest::TextToBinaryTest;

#define CASE(storage_class)                                               \
  do {                                                                    \
    EXPECT_THAT(                                                          \
        CompiledInstructions("OpTypeForwardPointer %pt " #storage_class), \
        Eq(MakeInstruction(spv::OpTypeForwardPointer,                     \
                           {1, StorageClass##storage_class})));           \
  } while (0)

TEST_F(OpTypeForwardPointerTest, ValidStorageClass) {
  CASE(UniformConstant);
  CASE(Input);
  CASE(Uniform);
  CASE(Output);
  CASE(WorkgroupLocal);
  CASE(WorkgroupGlobal);
  CASE(PrivateGlobal);
  CASE(Function);
  CASE(Generic);
  CASE(PushConstant);
  CASE(AtomicCounter);
  CASE(Image);
}

#undef CASE

TEST_F(OpTypeForwardPointerTest, MissingType) {
  EXPECT_THAT(CompileFailure("OpTypeForwardPointer"),
              Eq("Expected operand, found end of stream."));
}

TEST_F(OpTypeForwardPointerTest, MissingClass) {
  EXPECT_THAT(CompileFailure("OpTypeForwardPointer %pt"),
              Eq("Expected operand, found end of stream."));
}

TEST_F(OpTypeForwardPointerTest, WrongClass) {
  EXPECT_THAT(CompileFailure("OpTypeForwardPointer %pt xxyyzz"),
              Eq("Invalid storage class 'xxyyzz'."));
}

// TODO(dneto): error message test for sampler addressing mode
// TODO(dneto): error message test for sampler image format

// TODO(dneto): OpTypeVoid
// TODO(dneto): OpTypeBool
// TODO(dneto): OpTypeInt
// TODO(dneto): OpTypeFloat
// TODO(dneto): OpTypeVector
// TODO(dneto): OpTypeMatrix
// TODO(dneto): OpTypeImage
// TODO(dneto): OpTypeSampler
// TODO(dneto): OpTypeSampledImage
// TODO(dneto): OpTypeArray
// TODO(dneto): OpTypeRuntimeArray
// TODO(dneto): OpTypeStruct
// TODO(dneto): OpTypeOpaque
// TODO(dneto): OpTypePointer
// TODO(dneto): OpTypeFunction
// TODO(dneto): OpTypeEvent
// TODO(dneto): OpTypeDeviceEvent
// TODO(dneto): OpTypeReserveId
// TODO(dneto): OpTypeQueue

}  // anonymous namespace
