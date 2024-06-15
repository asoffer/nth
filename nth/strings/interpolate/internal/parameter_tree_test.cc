#include "nth/strings/interpolate/internal/parameter_tree.h"

#include <iostream>

#include "nth/strings/interpolate/string.h"
#include "nth/test/raw/test.h"

namespace {

void NoParameters() {
  nth::internal_interpolate::interpolation_parameter_tree<0> tree(
      nth::interpolation_string("abc"));
  (void)tree;
}

void OneParameter() {
  auto tree = nth::internal_interpolate::interpolation_parameter_tree<1>(
      nth::interpolation_string("{}"));
  auto param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 1);
  NTH_RAW_TEST_ASSERT(param.length == 0);
  NTH_RAW_TEST_ASSERT(param.width == 1);

  tree = nth::internal_interpolate::interpolation_parameter_tree<1>(
      nth::interpolation_string("abc{}"));
  param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 4);
  NTH_RAW_TEST_ASSERT(param.length == 0);
  NTH_RAW_TEST_ASSERT(param.width == 1);

  tree = nth::internal_interpolate::interpolation_parameter_tree<1>(
      nth::interpolation_string("abc{xyz}"));
  param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 4);
  NTH_RAW_TEST_ASSERT(param.length == 3);
  NTH_RAW_TEST_ASSERT(param.width == 1);
}

void NestedParameter() {
  auto tree = nth::internal_interpolate::interpolation_parameter_tree<2>(
      nth::interpolation_string("abc{x{y}z}"));
  auto param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 4);
  NTH_RAW_TEST_ASSERT(param.length == 5);
  NTH_RAW_TEST_ASSERT(param.width == 2);
}

void MultipleParameters() {
  auto tree = nth::internal_interpolate::interpolation_parameter_tree<3>(
      nth::interpolation_string("{}{}{}"));

  auto param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 1);
  NTH_RAW_TEST_ASSERT(param.length == 0);
  NTH_RAW_TEST_ASSERT(param.width == 1);

  param = tree.top_level_range<1>();
  NTH_RAW_TEST_ASSERT(param.start == 3);
  NTH_RAW_TEST_ASSERT(param.length == 0);
  NTH_RAW_TEST_ASSERT(param.width == 1);

  param = tree.top_level_range<2>();
  NTH_RAW_TEST_ASSERT(param.start == 5);
  NTH_RAW_TEST_ASSERT(param.length == 0);
  NTH_RAW_TEST_ASSERT(param.width == 1);
}

void MultipleParametersWithNesting() {
  auto tree = nth::internal_interpolate::interpolation_parameter_tree<8>(
      nth::interpolation_string("{x{}y{{}}z}{{}}{{}}"));
  auto param = tree.top_level_range<0>();
  NTH_RAW_TEST_ASSERT(param.start == 1);
  NTH_RAW_TEST_ASSERT(param.length == 9);
  NTH_RAW_TEST_ASSERT(param.width == 4);

  param = tree.top_level_range<1>();
  NTH_RAW_TEST_ASSERT(param.start == 12);
  NTH_RAW_TEST_ASSERT(param.length == 2);
  NTH_RAW_TEST_ASSERT(param.width == 2);

  param = tree.top_level_range<2>();
  NTH_RAW_TEST_ASSERT(param.start == 16);
  NTH_RAW_TEST_ASSERT(param.length == 2);
  NTH_RAW_TEST_ASSERT(param.width == 2);
}

}  // namespace

int main() {
  NoParameters();
  OneParameter();
  NestedParameter();
  MultipleParameters();
  MultipleParametersWithNesting();
}
