#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include <ns3/ndnSIM/apps/ndn-app.hpp>

namespace ns3 {
namespace ndn {

class ProducerApp : public App {
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("ProducerApp").SetParent<App>().AddConstructor<ProducerApp>();

    return tid;
  }

  ProducerApp()
  {
  }
  virtual ~ProducerApp()
  {
  }

  virtual void
  OnInterest(shared_ptr<const Interest> interest) override
  {
    std::cout << "Received INTEREST" << std::endl;
    std::cout << *interest << std::endl;

    Name dataName(interest->getName());
    dataName.append("hello").append("world");

    auto data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(::ndn::time::seconds(100));
    data->setContent(make_shared<::ndn::Buffer>(100));
    StackHelper::getKeyChain().sign(*data);

    std::cout << "node(" << GetNode()->GetId() << ") responding with Data: " << data->getName()
              << std::endl;

    m_transmittedDatas(data, this, m_face);
    m_appLink->onReceiveData(*data);
  }

  virtual void
  OnData(shared_ptr<const Data> data) override
  {
  }

protected:
  // from App
  virtual void
  StartApplication() override
  {
    App::StartApplication();
    FibHelper::AddRoute(GetNode(), "/", m_face, 0);
  }

  virtual void
  StopApplication() override
  {
    // do cleanup
    App::StopApplication();
    m_face->close();
  }
};

NS_OBJECT_ENSURE_REGISTERED(ProducerApp);

} // namespace ndn
} // namespace ns3
