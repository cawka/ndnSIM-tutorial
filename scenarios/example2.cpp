#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/point-to-point-grid.h>
#include <ns3/ndnSIM-module.h>

#include <ns3/ndnSIM/utils/tracers/ndn-app-delay-tracer.hpp>
#include <ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.hpp>
#include <ns3/ndnSIM/utils/tracers/ndn-cs-tracer.hpp>

namespace ns3 {

int
main(int argc, char* argv[])
{
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("20Kbps"));
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

  ndn::AppHelper cHelper("ns3::ndn::ConsumerZipfMandelbrot");
  cHelper.SetPrefix("/prefix");
  cHelper.SetAttribute("Frequency", StringValue("10"));
  cHelper.Install(consumerNodes);

  ndn::AppHelper pHelper("ns3::ndn::Producer");
  pHelper.SetPrefix("/prefix");
  pHelper.SetAttribute("PayloadSize", StringValue("1024"));
  pHelper.Install(producer);

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigin("/prefix", producer);

  // Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes();

  ndn::AppDelayTracer::InstallAll("results/app-delays-trace.txt");

  ndn::L3RateTracer::InstallAll("results/rate-trace.txt", Seconds(1.0));

  ndn::CsTracer::InstallAll("results/cs-trace.txt", Seconds(1));

  Simulator::Stop(Seconds(200.0));

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
