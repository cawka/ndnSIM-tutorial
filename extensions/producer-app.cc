
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"

using namespace ns3;
using namespace ns3::ndn;

class ProducerApp : public App
{
public:
  static TypeId GetTypeId ()
  {
    static TypeId tid = TypeId ("ProducerApp")
      .SetParent<App> ()
      .AddConstructor <ProducerApp> ()
      ;

    return tid;
  }

  ProducerApp () {}
  virtual ~ProducerApp () {}

  virtual void
  OnInterest (Ptr<const Interest> interest)
  {
    std::cout << "Received INTEREST" << std::endl;
    std::cout << *interest << std::endl;

    Ptr<Data> data = Create<Data> (Create<Packet> (100));
    Name name (interest->GetName ());
    name.append ("hello").append ("world");
    data->SetName (name);
    data->SetFreshness (Seconds (100.0));

    m_face->ReceiveData (data);
    m_transmittedDatas (data, this, m_face);
  }
 
protected:
  // from App
  virtual void
  StartApplication ()
  {
    App::StartApplication ();
    // send packet for example

    Ptr<Fib> fib = GetNode ()->GetObject<Fib> ();
    Ptr<fib::Entry> fibEntry = fib->Add (Name ("/"), m_face, 0);
    fibEntry->UpdateStatus (m_face, fib::FaceMetric::NDN_FIB_GREEN);
  }

  virtual void
  StopApplication ()
  {
    // do cleanup
    App::StopApplication ();   
  }
};

NS_OBJECT_ENSURE_REGISTERED (ProducerApp);
