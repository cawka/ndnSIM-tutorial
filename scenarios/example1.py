from ns.core import *
from ns.network import *
from ns.point_to_point import *
from ns.point_to_point_layout import *
from ns.ndnSIM import *

Config.SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("10Mbps"))
Config.SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"))
Config.SetDefault ("ns3::DropTailQueue::MaxPackets", StringValue ("20"))

import sys; cmd = CommandLine(); cmd.Parse (sys.argv);

p2p = PointToPointHelper()
grid = PointToPointGridHelper (3,3,p2p)
grid.BoundingBox(100,100,200,200)

# scenario meat
ndnHelper = ndn.StackHelper ()
ndnHelper.InstallAll();


# Getting containers for the consumer/producer
producer = grid.GetNode(2, 2)
consumerNodes = NodeContainer()
consumerNodes.Add (grid.GetNode(0,0))


cHelper = ndn.AppHelper("ns3::ndn::ConsumerCbr")
cHelper .SetPrefix("/prefix")
cHelper .SetAttribute("Frequency", StringValue ("10"))
cHelper .Install(consumerNodes)

pHelper = ndn.AppHelper("ns3::ndn::Producer")
pHelper.SetPrefix("/prefix")
pHelper.SetAttribute("PayloadSize", StringValue("1024"));
pHelper.Install(producer)

ndnGlobalRoutingHelper = ndn.GlobalRoutingHelper()
ndnGlobalRoutingHelper.InstallAll()

# Add /prefix origins to ndn::GlobalRouter
ndnGlobalRoutingHelper.AddOrigin("/prefix", producer)

# Calculate and install FIBs
ndnGlobalRoutingHelper.CalculateRoutes()

# Simulator.Stop (Seconds (20.0))
# Simulator.Run ()
# Simulator.Destroy ()

import visualizer
visualizer.start()
