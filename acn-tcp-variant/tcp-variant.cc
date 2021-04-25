/*

Topology used 

┌──────────┐                           ┌──────────┐                               ┌──────────┐
│ NODE 1             │   link 1                │ NODE 0            │   link 2                   │  NODE 2            │
│ SOURCE            ├──────────┤ GATEWAY          ├────────────┤  SINK                 │
└──────────┘                           └──────────┘                               └──────────┘


*/


#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpVariantsComparison");

bool firstCwnd = true;
bool firstSshThr = true;
bool firstRtt=true;
// output streams
Ptr<OutputStreamWrapper> cWndStream;
Ptr<OutputStreamWrapper> ssThreshStream;
Ptr<OutputStreamWrapper> ackStream;
Ptr<OutputStreamWrapper> congStateStream;
Ptr<OutputStreamWrapper> rttStream;

uint32_t cWndValue = 0;
uint32_t ssThreshValue = 0;
uint32_t rtovalue=0;


static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
    
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{  
  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }
}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
CongStateTracer (TcpSocketState::TcpCongState_t old, TcpSocketState::TcpCongState_t newState)
{
  *congStateStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newState << std::endl;
}

static void
AckTracer (SequenceNumber32 old, SequenceNumber32 newAck)
{
  *ackStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newAck << std::endl;
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream  = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}
static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
}  



static void
TraceAck (std::string &ack_file_name)
{
  AsciiTraceHelper ascii;
  ackStream = ascii.CreateFileStream (ack_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/HighestRxAck", MakeCallback (&AckTracer));
}

static void
TraceCongState (std::string &cong_state_file_name)
{
  AsciiTraceHelper ascii;
  congStateStream = ascii.CreateFileStream (cong_state_file_name.c_str ());
	Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongState", MakeCallback (&CongStateTracer));
}

int main (int argc, char *argv[])
{
  std::string transport_prot = "TcpWestwood";
  double error_p = 0.0;
  std::string bandwidth = "2Mbps";
  std::string delay = "0.01ms";
  std::string access_bandwidth = "10Mbps";
  std::string access_delay = "45ms";
  
  double data_mbytes = 0;
  uint32_t mtu_bytes = 400;
  float duration = 15;
  uint32_t run = 0;
  

  CommandLine cmd;
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
                 "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                 "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat", transport_prot);
  cmd.AddValue ("error_p", "Packet error rate", error_p);
  cmd.AddValue ("bandwidth", "Bottleneck bandwidth", bandwidth);
  cmd.AddValue ("delay", "Bottleneck bandwidth", delay);
  cmd.AddValue ("access_bandwidth", "Access link bandwidth", access_bandwidth);
  cmd.AddValue ("access_delay", "Access link delay", access_delay);
  cmd.AddValue ("data", "Number of Megabytes of data to transmit", data_mbytes);
  cmd.AddValue ("mtu", "Size of IP packets to send in bytes", mtu_bytes);

  cmd.Parse (argc, argv);
  NS_LOG_UNCOND(transport_prot+" Simulation");
  std::string file="scratch/acn-tcp-variant/"+transport_prot;
  mkdir(file.c_str(), 0777);
  SeedManager::SetSeed (1);
  SeedManager::SetRun (run);

  // User may find it convenient to enable logging
  LogComponentEnable("TcpVariantsComparison", LOG_LEVEL_ALL);


  // Calculate the ADU size
  Header* temp_header = new Ipv4Header ();
  uint32_t ip_header = temp_header->GetSerializedSize ();
  //NS_LOG_LOGIC ("IP Header size is: " << ip_header);
  delete temp_header;
  temp_header = new TcpHeader ();
  uint32_t tcp_header = temp_header->GetSerializedSize ();
  //NS_LOG_LOGIC ("TCP Header size is: " << tcp_header);
  delete temp_header;
  uint32_t tcp_adu_size = mtu_bytes - (ip_header + tcp_header);
  //NS_LOG_LOGIC ("TCP ADU size is: " << tcp_adu_size);

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));


  // Set the simulation start and stop time
  float start_time = 0.0;
  float stop_time = start_time + duration;
  transport_prot = std::string ("ns3::") + transport_prot;
  
  // Select TCP variant
   if (transport_prot.compare ("ns3::TcpWestwoodPlus") == 0)
     { 
       // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
       Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
       // the default protocol type in ns3::TcpWestwood is WESTWOOD
       Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
     }
   else
     {
       TypeId tcpTid;
       Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
     }

  // Create gateways, sources, and sinks
  NodeContainer gateways;
  gateways.Create (1);
  NodeContainer sources;
  sources.Create (1);
  NodeContainer sinks;
  sinks.Create (1);
 
 
  // Configure the error model
  // Here we use RateErrorModel with packet error rate
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);

  


  InternetStackHelper stack;
  stack.InstallAll ();

  

  // Configure the sources and sinks net devices
  // and the channels between the sources/sinks and the gateways
  PointToPointHelper link1;
  link1.SetDeviceAttribute ("DataRate", StringValue (access_bandwidth));
  link1.SetChannelAttribute ("Delay", StringValue (access_delay));

  PointToPointHelper link2;
  link2.SetDeviceAttribute ("DataRate", StringValue (bandwidth));
  link2.SetChannelAttribute ("Delay", StringValue (access_delay));
  link2.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));

  
 

  NetDeviceContainer devices1,devices2;
  devices1 = link1.Install (sources.Get (0), gateways.Get (0));
  devices2=link2.Install(gateways.Get (0), sinks.Get (0));

  Ipv4AddressHelper address;
  
  
  
  Ipv4InterfaceContainer interfaces1,interfaces2;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  interfaces1 = address.Assign (devices1);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  interfaces2= address.Assign (devices2);

  

  //NS_LOG_INFO ("Initialize Global Routing.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t port = 2430;

  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));

  
  AddressValue remoteAddress (InetSocketAddress (interfaces2.GetAddress (1), port));

 // source traffic genrator
  BulkSendHelper source ("ns3::TcpSocketFactory", Address ());



  source.SetAttribute ("Remote", remoteAddress);
  source.SetAttribute ("SendSize", UintegerValue (tcp_adu_size));
  // 0 means unlimited
  source.SetAttribute ("MaxBytes", UintegerValue (int(data_mbytes * 1000000)));
  

  ApplicationContainer sourceApp = source.Install (sources.Get (0));

  sourceApp.Start (Seconds (start_time));
  sourceApp.Stop (Seconds (stop_time-3));


  
  ApplicationContainer sinkApp = sinkHelper.Install (sinks);
  sinkApp.Start (Seconds (start_time));
  sinkApp.Stop (Seconds (stop_time));

      

  // Set up tracing 
  AsciiTraceHelper ascii;
  stack.EnableAsciiIpv4All (ascii.CreateFileStream(file+"/ascii.tr"));
  Simulator::Schedule (Seconds (0.00001), &TraceCwnd, file + "/cwnd.data");
  Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, file + "/ssth.data");
  Simulator::Schedule (Seconds (0.00001), &TraceAck, file + "/ack.data");
  Simulator::Schedule (Seconds (0.00001), &TraceCongState, file + "/cong-state.data");
  Simulator::Schedule (Seconds (0.00001), &TraceRtt, file + "/rtt.data");

  link2.EnablePcapAll (file+"/link2", true);
  link1.EnablePcapAll (file+"/link1", true);

  // Flow monitor
  FlowMonitorHelper flowHelper;
  flowHelper.InstallAll ();
    
  //net anim
  AnimationInterface anim(file+"/anim.xml"); //to save file for netAnim
 anim.UpdateNodeDescription(sources.Get(0),"SOUCRE");

 anim.UpdateNodeDescription(gateways.Get(0),"Gateway");

 anim.UpdateNodeDescription(sinks.Get(0),"SINK");

  Simulator::Stop (Seconds (stop_time));
  Simulator::Run ();
  flowHelper.SerializeToXmlFile (file+"/flow.xml", true, true);
  Simulator::Destroy ();
  return 0;
}