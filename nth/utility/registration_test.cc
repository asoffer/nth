#include "nth/utility/registration.h"

#include "gtest/gtest.h"

struct AbstractThing {
  virtual int DoSomething() const = 0;
};
NTH_DECLARE_REGISTRATION_POINT(AbstractThing);

struct ConcreteThing : AbstractThing {
  int DoSomething() const override { return 1; }
};
NTH_REGISTER(AbstractThing, ConcreteThing);

TEST(Registration, CanBeInvoked) {
  EXPECT_EQ(nth::registry.get<AbstractThing>().DoSomething(), 1);
}


struct AnonymousNamespaceAbstractThing {
  virtual int DoSomething() const = 0;
};
NTH_DECLARE_REGISTRATION_POINT(AnonymousNamespaceAbstractThing);

namespace {
struct AnonymousNamespaceConcreteThing : AnonymousNamespaceAbstractThing {
  int DoSomething() const override { return 2; }
};
}  // namespace

NTH_REGISTER(AnonymousNamespaceAbstractThing, AnonymousNamespaceConcreteThing);

TEST(Registration, WorksInAnonymousNamespace) {
  EXPECT_EQ(nth::registry.get<AnonymousNamespaceAbstractThing>().DoSomething(),
            2);
}
