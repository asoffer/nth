#include "nth/dynamic/jit_function.h"

#include "nth/base/platform.h"
#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("jit_function/default-construction") {
  jit_function<void()> f;
  NTH_EXPECT(not f);
  NTH_EXPECT(static_cast<void (*)()>(f) == nullptr);
}

std::span<std::byte const> SquareFunctionCode() {
  static constexpr uint8_t buffer[] =
#if NTH_ARCHITECTURE(x64)
      {
          0x0f, 0xaf, 0xff,  // imul edi, edi
          0x89, 0xf8,        // mov eax, edi
          0xc3,              // ret
      };
#elif NTH_ARCHITECTURE(arm64)
      {
          0x00, 0x7c, 0x00, 0x1b,  // mul w0, w0, w0
          0xc0, 0x03, 0x5f, 0xd6,  // ret
      };
#else  // NTH_ARCHITECTURE
#error Unsupported architecture.
#endif  // NTH_ARCHITECTURE

  return std::span<std::byte const>(reinterpret_cast<std::byte const*>(buffer),
                                    sizeof(buffer));
}

NTH_TEST("jit_function/success") {
  jit_function<int(int)> square(SquareFunctionCode());
  NTH_ASSERT(static_cast<bool>(square));
  NTH_ASSERT(static_cast<int (*)(int)>(square) != nullptr);
  NTH_ASSERT(static_cast<int (*)(int)>(square)(17) == 289);
  NTH_ASSERT(square(17) == 289);
}

// TODO: Inject behavior into internals so we can test the failure paths.

}  // namespace
}  // namespace nth
