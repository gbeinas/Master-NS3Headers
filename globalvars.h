/* A file to hold the global variabes of the main simulations. */

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "ns3/network-module.h"

using namespace ns3;

// Lte + EPC Helper and various containers.
Ptr <LteHelper> lteHelper = CreateObject<LteHelper> ();
Ptr<PointToPointEpcHelper> epcHelper;
NodeContainer homeEnbs,macroEnbs,GSMEnbs,wlanAPs,ues;
NetDeviceContainer allDevs;
NetDeviceContainer macroEnbDevs,homeEnbDevs,GSMEnbDevs,wlanDevs,ueDevs;
Ptr<LteUeNetDevice> uePtr;
Ptr<RadioBearerStatsCalculator> pdcpStats;

// Ports and Application Containers
uint16_t dlPort = 10000;
uint16_t ulPort = 30000;
ApplicationContainer clientApps, cdataApps;
ApplicationContainer serverApps, sdataApps;

// SETUP
uint32_t nMacroEnbs = 1;
uint32_t nHomeEnbs = 10;
uint32_t nWiFiAC = 10;
const uint32_t nUEs = 10;  // test UEs + rest UEs
double UeTxPower = 20;

/* nBS  is used to parse the callback path correctly for the UEs, e.g. if there are 2 eNBs and 5 HeNBs, UE IMSI 1 is /Nodelist/7, if there are 2 eNBs, 6 HeNBs, UE IMSI 1 is /Nodelist/8 etc. */
uint32_t nBS = nMacroEnbs + nHomeEnbs;

// Service Types
uint16_t stype;

// EPC and routing containers
Ipv4Address remoteHostAddr;
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ipv4InterfaceContainer ueIpIfaces;
Ptr<Node> remoteHost;
NodeContainer remoteHostContainer;


/* Static Global Values */
static ns3::GlobalValue g_macroEnbTxPowerDbm ("macroEnbTxPowerDbm",
                                              "TX power [dBm] used by macro eNBs",
                                              ns3::DoubleValue (35.0),
                                              ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_homeEnbTxPowerDbm ("homeEnbTxPowerDbm",
                                             "TX power [dBm] used by HeNBs",
                                             ns3::DoubleValue (10.0),
                                             ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_macroEnbDlEarfcn ("macroEnbDlEarfcn",
                                            "DL EARFCN used by macro eNBs",
                                            ns3::UintegerValue (300),
                                            ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_GSMEnbDlEarfcn ("GSMEnbDlEarfcn",
                                            "DL EARFCN used by GSM eNBs",
                                            ns3::UintegerValue (3500),
                                            ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_homeEnbDlEarfcn ("homeEnbDlEarfcn",
                                           "DL EARFCN used by HeNBs",
                                           ns3::UintegerValue (100),
                                           ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_macroEnbBandwidth ("macroEnbBandwidth",
                                             "bandwidth [num RBs] used by macro eNBs",
                                             ns3::UintegerValue (50),
                                             ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_homeEnbBandwidth ("homeEnbBandwidth",
                                            "bandwidth [num RBs] used by HeNBs",
                                            ns3::UintegerValue (15),
                                            ns3::MakeUintegerChecker<uint16_t> ());
static ns3::GlobalValue g_simTime ("simTime",
                                   "Total duration of the simulation [s]",
                                   ns3::DoubleValue (10.0),
                                   ns3::MakeDoubleChecker<double> ());
static ns3::GlobalValue g_generateRem ("generateRem",
                                       "if true, will generate a REM and then abort the simulation;"
                                       "if false, will run the simulation normally (without generating any REM)",
                                       ns3::BooleanValue (false),
                                       ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_epc ("epc",
                               "If true, will setup the EPC to simulate an end-to-end topology, "
                               "with real IP applications over PDCP and RLC UM (or RLC AM by changing "
                               "the default value of EpsBearerToRlcMapping e.g. to RLC_AM_ALWAYS). "
                               "If false, only the LTE radio access will be simulated with RLC SM. ",
                               ns3::BooleanValue (true),
                               ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_epcDl ("epcDl",
                                 "if true, will activate data flows in the downlink when EPC is being used. "
                                 "If false, downlink flows won't be activated. "
                                 "If EPC is not used, this parameter will be ignored.",
                                 ns3::BooleanValue (true),
                                 ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_epcUl ("epcUl",
                                 "if true, will activate data flows in the uplink when EPC is being used. "
                                 "If false, uplink flows won't be activated. "
                                 "If EPC is not used, this parameter will be ignored.",
                                 ns3::BooleanValue (true),
                                 ns3::MakeBooleanChecker ());
static ns3::GlobalValue g_fadingTrace ("fadingTrace",
                                           "The path of the fading trace (by default no fading trace "
                                           "is loaded, i.e., fading is not considered)",
                                           ns3::StringValue (""),
                                           ns3::MakeStringChecker ());
static ns3::GlobalValue g_srsPeriodicity ("srsPeriodicity",
                                               "SRS Periodicity (has to be at least greater than the number of UEs per eNB)",
                                               ns3::UintegerValue (160),
                                               ns3::MakeUintegerChecker<uint16_t> ());


#endif
