#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/point-to-point-grid.h>
#include <ns3/ndnSIM-module.h>

int
main(int argc, char* argv[])
{
  ns3::Config::SetDefault("ns3::PointToPointNetDevice::DataRate", ns3::StringValue("10Mbps"));
  ns3::Config::SetDefault("ns3::PointToPointChannel::Delay", ns3::StringValue("10ms"));
  ns3::Config::SetDefault("ns3::DropTailQueue::MaxPackets", ns3::StringValue("20"));

  ns3::CommandLine cmd;
  cmd.Parse(argc, argv);

  ns3::PointToPointHelper p2p;
  ns3::PointToPointGridHelper grid(3, 3, p2p);
  grid.BoundingBox(100, 100, 200, 200);

  // scenario meat
  ns3::ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Example", "MaxSize", "10");
  ndnHelper.InstallAll();

  // Getting containers for the consumer/producer
  ns3::Ptr<ns3::Node> producer = grid.GetNode(2, 2);
  ns3::NodeContainer consumerNodes;
  consumerNodes.Add(grid.GetNode(0, 0));

  ns3::ndn::AppHelper cHelper("ns3::ndn::ConsumerCbr");
  cHelper.SetPrefix("/prefix");
  cHelper.SetAttribute("Frequency", ns3::StringValue("10"));
  cHelper.Install(consumerNodes);

  ns3::ndn::AppHelper pHelper("ns3::ndn::Producer");
  pHelper.SetPrefix("/prefix");
  pHelper.SetAttribute("PayloadSize", ns3::StringValue("1024"));
  pHelper.Install(producer);

  ns3::ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigin("/prefix", producer);

  // Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes();

  ns3::Simulator::Stop(ns3::Seconds(20.0));

  ns3::Simulator::Run();
  ns3::Simulator::Destroy();

  return 0;
}
