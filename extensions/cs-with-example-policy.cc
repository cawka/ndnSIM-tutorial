#include "example-policy.h"
#include "ns3/ndnSIM/model/cs/content-store-impl.h"

using namespace ns3;
using namespace ns3::ndn;


static struct InstantiationContentStoreImplWithExamplePolicy
{
  InstantiationContentStoreImplWithExamplePolicy ()
  {
    TypeId tid = cs::ContentStoreImpl<example_policy_traits>::GetTypeId ();
    tid.GetParent ();
  }
} x_InstantiationContentStoreImplWithExamplePolicy;
