#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"
#include <ns3/ndnSIM/apps/ndn-app.hpp>

namespace ns3 {
namespace ndn {

class RequesterApp : public App {
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("RequesterApp").SetParent<App>().AddConstructor<RequesterApp>();

    return tid;
  }

  RequesterApp()
  {
  }
  virtual ~RequesterApp()
  {
  }

  virtual void
  OnData(shared_ptr<const Data> data) override
  {
    std::cout << "Received DATA" << std::endl;
    std::cout << *data << std::endl;
  }

protected:
  // from App
  virtual void
  StartApplication() override
  {
    App::StartApplication();
    // send packet for example

    Simulator::ScheduleNow(&RequesterApp::ExpressSomeInterest, this);
  }

  virtual void
  StopApplication() override
  {
    // do cleanup
    App::StopApplication();
    m_face->close();
  }

private:
  void
  ExpressSomeInterest()
  {
    auto interest = make_shared<Interest>();
    interest->setName(Name("/prefix/hello/world"));
    interest->setInterestLifetime(time::seconds(1));

    // m_transmittedInterests (interest, this, m_face);
    // m_face->ReceiveInterest (interest);
  }
};

NS_OBJECT_ENSURE_REGISTERED(RequesterApp);

} // namespace ndn
} // namespace ns3
