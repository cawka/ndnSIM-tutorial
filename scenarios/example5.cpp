#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/point-to-point-grid.h>
#include <ns3/ndnSIM-module.h>

#include <ndn-cxx/face.hpp>

class ConsumerApp {
public:
  void
  Run()
  {
    m_face = make_shared<ndn::Face>();

    m_face->expressInterest(ndn::Interest("/prefix/hello/world"),
                            bind(&ConsumerApp::gotData, this, _1, _2), nullptr,
                            bind(&ConsumerApp::gotTimeout, this, _1));

    m_face->expressInterest(ndn::Interest("/other-prefix/hello"),
                            bind(&ConsumerApp::gotData, this, _1, _2), nullptr,
                            bind(&ConsumerApp::gotTimeout, this, _1));
  }

private:
  void
  gotData(const ndn::Interest& interest, const ndn::Data& data)
  {
    std::cout << "GOT DATA FOR: " << interest << std::endl;
    std::cout << data << std::endl;
  }

  void
  gotTimeout(const ndn::Interest& interest)
  {
    std::cout << "GOT TIMEOUT FOR: " << interest << std::endl;
  }

private:
  shared_ptr<ndn::Face> m_face;
};

class ProducerApp {
public:
  void
  Run()
  {
    m_face = make_shared<ndn::Face>();
    m_face->setInterestFilter(ndn::Name("/prefix"), bind(&ProducerApp::gotInterest, this, _1, _2),
                              nullptr);
  }

private:
  void
  gotInterest(const ndn::InterestFilter&, const ndn::Interest& interest)
  {
    std::cout << "GOT INTEREST: " << interest << std::endl;

    ndn::Name dataName(interest.getName());
    dataName.append("hello").append("world");

    auto data = make_shared<ndn::Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(::ndn::time::seconds(100));
    data->setContent(make_shared<::ndn::Buffer>(100));
    ns3::ndn::StackHelper::getKeyChain().sign(*data);

    m_face->put(*data);
  }

private:
  shared_ptr<ndn::Face> m_face;
};

namespace ns3 {
namespace ndn {

int
main(int argc, char* argv[])
{
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("10Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

  CommandLine cmd;
  cmd.Parse(argc, argv);

  PointToPointHelper p2p;
  PointToPointGridHelper grid(3, 3, p2p);
  grid.BoundingBox(100, 100, 200, 200);

  // scenario meat
  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> producerNode = grid.GetNode(2, 2);
  Ptr<Node> consumerNode = grid.GetNode(0, 0);

  ConsumerApp consumer;
  Simulator::ScheduleWithContext(consumerNode->GetId(), Seconds(2.0), &ConsumerApp::Run, &consumer);

  ProducerApp producer;
  Simulator::ScheduleWithContext(producerNode->GetId(), Seconds(1.0), &ProducerApp::Run, &producer);

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigin("/", producerNode);

  // Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ndn
} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::ndn::main(argc, argv);
}
