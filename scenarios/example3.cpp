#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/point-to-point-grid.h>
#include <ns3/ndnSIM-module.h>

namespace ns3 {

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
  Ptr<Node> producer = grid.GetNode(2, 2);
  NodeContainer consumerNodes;
  consumerNodes.Add(grid.GetNode(0, 0));

  ndn::AppHelper cHelper("RequesterApp");
  cHelper.Install(consumerNodes);

  ndn::AppHelper pHelper("ns3::ndn::Producer");
  pHelper.SetPrefix("/prefix");
  pHelper.SetAttribute("PayloadSize", StringValue("1024"));
  pHelper.Install(producer);

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigin("/", producer);

  // Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
