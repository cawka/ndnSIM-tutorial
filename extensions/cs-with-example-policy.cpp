#include "example-policy.hpp"
#include "ns3/ndnSIM/model/cs/content-store-impl.hpp"

namespace ns3 {
namespace ndn {

static struct InstantiationContentStoreImplWithExamplePolicy {
  InstantiationContentStoreImplWithExamplePolicy()
  {
    TypeId tid = cs::ContentStoreImpl<example_policy_traits>::GetTypeId();
    tid.GetParent();
  }
} x_InstantiationContentStoreImplWithExamplePolicy;

} // namespace ndn
} // namespace ns3
