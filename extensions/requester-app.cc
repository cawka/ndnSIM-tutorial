
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"

using namespace ns3;
using namespace ns3::ndn;

class RequesterApp : public App
{
public:
  static TypeId GetTypeId ()
  {
    static TypeId tid = TypeId ("RequesterApp")
      .SetParent<App> ()
      .AddConstructor <RequesterApp> ()
      ;

    return tid;
  }

  RequesterApp () {}
  virtual ~RequesterApp () {}

  virtual void
  OnData (Ptr<const Data> data)
  {
    std::cout << "Received DATA" << std::endl;
    std::cout << *data << std::endl;
  }
 
protected:
  // from App
  virtual void
  StartApplication ()
  {
    App::StartApplication ();
    // send packet for example

    Simulator::ScheduleNow (&RequesterApp::ExpressSomeInterest, this);
  }

  virtual void
  StopApplication ()
  {
    // do cleanup
    App::StopApplication ();   
  }

private:
  void
  ExpressSomeInterest ()
  {
    Ptr<Interest> interest = Create<Interest> ();
    interest->SetName (Name ("/prefix/hello/world"));
    interest->SetNonce (10);
    interest->SetInterestLifetime (Seconds (1));

    m_transmittedInterests (interest, this, m_face);
    m_face->ReceiveInterest (interest);
  }
};

NS_OBJECT_ENSURE_REGISTERED (RequesterApp);
