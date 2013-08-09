#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-grid.h"
#include "ns3/ndnSIM-module.h"

using namespace ns3;
using namespace ns3::ndn;

class ConsumerApp
{
public:
  void
  Run (Ptr<Node> node)
  {
    m_face = CreateObject<ApiFace> (node);
    Ptr<Interest> i = Create<Interest> ();
    i->SetName (Name ("/prefix/hello/world"));
    
    m_face->ExpressInterest (i, MakeCallback (&ConsumerApp::GotData, this),
                             MakeCallback (&ConsumerApp::GotTimeout, this));

    i->SetName (Name ("/other-prefix/hello"));
    m_face->ExpressInterest (i, MakeCallback (&ConsumerApp::GotData, this),
                             MakeCallback (&ConsumerApp::GotTimeout, this));
  }
  
private:
  void
  GotData (Ptr<const Interest> interest, Ptr<const Data> data)
  {
    std::cout << "GOT DATA FOR: " << *interest << std::endl;
    std::cout << *data << std::endl;
  }

  void
  GotTimeout (Ptr<const Interest> interest)
  {
    std::cout << "GOT TIMEOUT FOR: " << *interest << std::endl;
  }

private:
  Ptr<ApiFace> m_face;
};

class ProducerApp
{
public:
  void
  Run (Ptr<Node> node)
  {
    m_face = CreateObject<ApiFace> (node);
    m_face->SetInterestFilter (Create<Name> ("/prefix"), MakeCallback (&ProducerApp::GotInterest, this));
  }
  
private:
  void
  GotInterest (Ptr<const Name> name, Ptr<const Interest> interest)
  {
    std::cout << "GOT INTEREST: " << *interest << std::endl;
    
    Ptr<Data> data = Create<Data> (Create<Packet> (100));
    data->SetName (Name (interest->GetName ()).append ("hello").append ("world"));
    data->SetFreshness (Seconds (100.0));

    m_face->Put (data);
  }

private:
  Ptr<ApiFace> m_face;
};

int
main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("10Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("20"));

  CommandLine cmd; cmd.Parse (argc, argv);

  PointToPointHelper p2p;
  PointToPointGridHelper grid (3, 3, p2p);
  grid.BoundingBox(100,100,200,200);
   
  // scenario meat
  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll ();

  // Getting containers for the consumer/producer
  Ptr<Node> producerNode = grid.GetNode (2, 2);
  Ptr<Node> consumerNode = grid.GetNode (0, 0);

  ConsumerApp consumer;
  Simulator::Schedule (Seconds (1.0), &ConsumerApp::Run, &consumer, consumerNode);

  ProducerApp producer;
  Simulator::ScheduleNow (&ProducerApp::Run, &producer, producerNode);
  
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll ();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigin ("/", producerNode);

  // Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes ();

  Simulator::Stop (Seconds (20.0));

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
