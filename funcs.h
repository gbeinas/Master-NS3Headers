/* A file to hold auxiliary functions to the main simulations. */

#ifndef FUNCS_H
#define FUNCS_H

#include <vector>
#include <cmath>
//#include <ns3/globalvars.h>
#include "ns3/random-variable-stream.h"

#define VOICE 0
#define VIDEO 1
#define VOIP 2
#define WEBDATA 1
#define STREAMDATA 2
#define IDLE 3

using namespace ns3;
using namespace std;

/* ----------------Command-Line Variables --------------*/
CommandLine cmd;
bool useCa = true;
int RngSize = 250;
double simTime = 15.0; /* Measured in seconds*/ //100
string ul_pdcp_flname = "Master_PDCP-UL.txt";
string dl_pdcp_flname = "Master_PDCP-DL.txt";

/* ----------------Arithmetic Variables --------------*/
static uint16_t nHeNbs = 10;
static uint16_t neNbs = 4;
static uint16_t nWiFi = 20;
uint32_t nUEs = 10;
uint32_t nUEs_Array = nUEs;
double packet_interval = 3.8;
int voice_packet_size = 100;
int voip_packet_size = 100;
double TxPower = 23.0;
double frequency = 2.5;
double internalWallLoss = 5.0;
double shadow = 8.0;
double eNB_dl_earfcn = 100.0;
double eNB_ul_earfcn = 18000.0;
double HeNB_dl_earfcn = 100.0;
double Henb_ul_earfcn = 18000.0;
double eNbDLBandwidth = 15; //Value is equivalent to Resource Blocks
double eNbULBandwidth = 15; //Value is equivalent to Resource Blocks
double HeNbDLBandwidth = 6; //Value is equivalent to Resource Blocks
double HeNbULBandwidth = 6; //Value is equivalent to Resource Blocks
uint16_t dlPort = 10000;
uint16_t ulPort = 30000;

/* ------------- struct to store UE stats  -------------- */
struct ueInformation{
	uint32_t ue_id;
	uint64_t txBytes [3];
	uint64_t rxBytes [3];
	uint32_t txPackets [3];
	uint32_t rxPackets [3];
	uint32_t LostPackets [3];
	double transition_time;
};

/* --------------------- Load Traffic Variables ------------------------- */
ueInformation* ueIds;
int* CellMap; //where each UE is attached every time
uint32_t* Ues_per_Cell;

/* --------------- Initial Container-Helpers -------------*/
Ptr<Building> build;
Ptr<LteHelper> lteHelper;
static Ptr<EpcHelper> epcHelper;
Ptr<MobilityBuildingInfo> mbi;
Ptr<Node> remoteHost;
MobilityHelper mobility;
NetDeviceContainer enbLteDevs,ueLteDevs,HenbLteDevs,alleNbs;;
NodeContainer eNbsnodes, HeNbsnodes, UEsnodes, remoteHostContainer, WiFinodes;
Ptr<ConstantPositionMobilityModel> mm0;
Ptr<MobilityModel> mob;
Vector pos;
InternetStackHelper internet;
Ipv4InterfaceContainer ueIpIface;
Ipv4AddressHelper address;
Ipv4StaticRoutingHelper ipv4RoutingHelper;
Ipv4Address remoteHostAddr;
Ipv4AddressHelper address;
Ipv4InterfaceContainer internetIpIfaces;
Ptr<Node> pgw ;
static Ptr<LteEnbNetDevice> eNbLteDevice;

/*--------------- Various Variables Declaration-----------*/
Ptr<NormalRandomVariable> endTimeVoice;
Ptr<NormalRandomVariable> endTimeVideo;
Ptr<NormalRandomVariable> endTimeVoip;
Ptr<UniformRandomVariable> sTypes;


/*********************** Utility Funtions ************************/
/*
void AppsSetup (Ptr<Node> ue, uint16_t &dlPort, uint16_t &ulPort, uint32_t u, Time startTime, Time endTime, uint16_t stype)
{
  if(stype == IDLE)
    return;

  ApplicationContainer cApps, sApps;

  // Set the default gateway for the UE
  Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
  ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

  ++ulPort;
  ++dlPort;
  PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
  PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
  sApps.Add (dlPacketSinkHelper.Install (ues.Get(u)));
  sApps.Add (ulPacketSinkHelper.Install (remoteHost));

  UdpClientHelper dlClient (ueIpIfaces.GetAddress (u), dlPort);
  dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(10)));
  dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
  if (stype == VOICE)
    dlClient.SetAttribute ("PacketSize", UintegerValue(18));
  else if (stype == WEBDATA)
    dlClient.SetAttribute ("PacketSize", UintegerValue(240));
  else if (stype == STREAMDATA)
    dlClient.SetAttribute ("PacketSize", UintegerValue(1220));

  if (stype == VOICE) {
    UdpClientHelper ulClient (remoteHostAddr, ulPort);
    ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(10)));
    ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
    ulClient.SetAttribute ("PacketSize", UintegerValue(18));
    cApps.Add (ulClient.Install (ue));
  }

  cApps.Add (dlClient.Install (remoteHost));

  sApps.Start (startTime);
  cApps.Start (startTime);
  sApps.Stop (endTime);
  cApps.Stop (endTime);
}

void AllocateFemtocells (uint32_t nHomeEnbs, Ptr<ListPositionAllocator> HomeEnbPositionAlloc, Box buildinfo) {

  int xstart = buildinfo.xMin+10;
  int ystart = buildinfo.yMin+10;
  int xend = buildinfo.xMax-10;
  int yend = buildinfo.yMax-10;
  uint32_t perrow = nHomeEnbs / 2;
  int xdist = (xend - xstart) / (perrow-1);

  for (uint32_t i=0; i<perrow; i++) {
    HomeEnbPositionAlloc->Add(Vector ((i*xdist)+10, ystart, 1));
    cout << "START" << Vector ((i*xdist)+10, ystart, 1) << endl;
    HomeEnbPositionAlloc->Add(Vector ((i*xdist)+10, yend, 1));
    cout << "END" << Vector ((i*xdist)+10, yend, 1) << endl;
  }
}

void
PrintGnuplottableBuildingListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      //NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  uint32_t index = 0;
  for (BuildingList::Iterator it = BuildingList::Begin (); it != BuildingList::End (); ++it)
    {
      ++index;
      Box box = (*it)->GetBoundaries ();
      outFile << "set object " << index
              << " rect from " << box.xMin  << "," << box.yMin
              << " to "   << box.xMax  << "," << box.yMax
              << " front fs empty "
              << endl;
    }
} */

void PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      //NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
          if (uedev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at "<< pos.x << "," << pos.y << " left font \"Helvetica,8\" textcolor rgb \"grey\" front point pt 1 ps 0.3 lc rgb \"grey\" offset 0,0"
                      << endl;
            }
        }
    }
}

void
PrintGnuplottableEnbListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      //NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<LteEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <LteEnbNetDevice> ();
          if (enbdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << enbdev->GetCellId ()
                      << "\" at "<< pos.x << "," << pos.y
                      << " left font \"Helvetica,8\" textcolor rgb \"white\" front  point pt 2 ps 0.3 lc rgb \"white\" offset 0,0"
                      << endl;
            }
        }
    }
}

void createBuilding(double xstart, double xend, double ystart, double yend, double zmin, double zmax, uint16_t nRoomsX, uint16_t nRoomsY, uint16_t nFloors,Ptr<Building> build,Ptr<MobilityBuildingInfo> mbi){

	cout <<" ******* Creating Building ******* "<< endl;
	build->SetBoundaries(Box(xstart, xend, ystart, yend, zmin, zmax));
	build->SetBuildingType(Building::Commercial);
	build->SetExtWallsType(Building::ConcreteWithWindows);
	build->SetNFloors(1);
	build->SetNRoomsX(5);
	build->SetNRoomsY(2);
	mbi = CreateObject<MobilityBuildingInfo>();
	cout<< "The boundaries of the building is: " << build->GetBoundaries() << " ,number of floors is: " << build->GetNFloors() << ", the number of rooms in X is : " << build->GetNRoomsX() << " and in Y is : " << build->GetNRoomsY() << endl;
}

double getRandom (double min,double max)
{
		Ptr<UniformRandomVariable> sTypes = CreateObject<UniformRandomVariable> ();
		sTypes->SetAttribute ("Min", DoubleValue (min));
		sTypes->SetAttribute ("Max", DoubleValue (max));
		return sTypes->GetValue();
}

void createMobility (MobilityHelper mobility, Ptr<ConstantPositionMobilityModel> mm0, uint16_t neNbs, uint16_t nHeNbs, uint16_t nUEs, uint16_t nWiFi, NodeContainer& eNbsnodes, NodeContainer& HeNbsnodes, NodeContainer& UEsnodes, NodeContainer& WiFinodes, NodeContainer& remoteHostContainer,double xstart, double xend, double ystart, double yend)
{
//--------------------------------Create all lte/wifi nodes------------------------------//
		UEsnodes.Create(nUEs);
		eNbsnodes.Create(neNbs);
		HeNbsnodes.Create(nHeNbs);
		remoteHostContainer.Create(1);

		/* TODO Add extra functionality for WiFi nodes */
		WiFinodes.Create(nWiFi);

		cout << "Just created : " << eNbsnodes.GetN() << " eNbs!!" << "\n" << endl;
		cout << "Just created : " << HeNbsnodes.GetN() << " HeNbs!!" << "\n" << endl;
		cout << "Just created : " << WiFinodes.GetN() << " WiFi AP!!" << "\n" << endl;
		cout << "Just created : " << UEsnodes.GetN() << " UE nodes!!" << "\n" << endl;

		//-----------------------Declaration of Mobility and Position for All Nodes------------------------//

		//Create Mobility and Position of Access Points//
		mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
		mobility.Install(HeNbsnodes);
		BuildingsHelper::Install(HeNbsnodes);
		mobility.Install(eNbsnodes);
		BuildingsHelper::Install(eNbsnodes);
		mobility.Install(remoteHostContainer);
		BuildingsHelper::Install(remoteHostContainer);
		mobility.Install(WiFinodes);
		BuildingsHelper::Install(WiFinodes);

		/* ---------------------HeNBs topology --------------------------*/
		mm0 = HeNbsnodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(305,320),getRandom(310 ,325), 3));

		mm0 = HeNbsnodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(330,345),getRandom(310 ,325), 3));

		mm0 = HeNbsnodes.Get(2)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(355,370),getRandom(310 ,325), 3));

		mm0 = HeNbsnodes.Get(3)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(380,395),getRandom(310 ,325), 3));

		mm0 = HeNbsnodes.Get(4)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(305,320),getRandom(330 ,345), 3));

		mm0 = HeNbsnodes.Get(5)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(330,345),getRandom(330 ,345), 3));

		mm0 = HeNbsnodes.Get(6)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(355,370),getRandom(330 ,345), 3));

		mm0 = HeNbsnodes.Get(7)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(380,395),getRandom(330 ,345), 3));

		mm0 = HeNbsnodes.Get(8)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(303,397),getRandom(303 ,347), 3));

		mm0 = HeNbsnodes.Get(9)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(303,397),getRandom(303 ,347), 3));


		/* ---------------------eNBs topology --------------------------*/
		mm0 = eNbsnodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(130,200),getRandom(430 ,550),10));

		mm0 = eNbsnodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(130,200),getRandom(100 ,200),10));

		mm0 = eNbsnodes.Get(2)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(390,460),getRandom(430 ,550),10));

		mm0 = eNbsnodes.Get(3)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(getRandom(430,500),getRandom(100 ,200),10));

		mm0 =remoteHostContainer.Get(0)->GetObject<ConstantPositionMobilityModel>();
		mm0->SetPosition(Vector(350, 325, 3));

		Ptr<PositionAllocator> positionAlloc = CreateObject<RandomRoomPositionAllocator>();
		mobility.SetPositionAllocator(positionAlloc);

		//mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel","Bounds",StringValue ("200|300|200|250|1|4"),"Speed",StringValue("ns3::UniformRandomVariable[Min=0.02][Max=0.55]"));
		mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds",RectangleValue (Rectangle (xstart, xend, ystart, yend)), "Speed",StringValue("ns3::UniformRandomVariable[Min=2.0][Max=4.0]"));

		for (uint16_t i = 0; i < nUEs; i++) {
			mobility.Install(UEsnodes.Get(i));
		}

		BuildingsHelper::Install(UEsnodes);
		BuildingsHelper::MakeMobilityModelConsistent();
		cout << "Mobility created with success!!" << "\n"<< endl;

}

/**
 * Installs a set of (H)eNBs to the access network
 */
NetDeviceContainer accessNetworkSetUp(const NodeContainer& nodes, double TxPower, string pathLossModel, double frequency, double internalWallLoss, double shadow, double dl_earfcn, double ul_earfcn, double dl_bw, double ul_bw) {
	/*----------------------------------eNBs Declaration----------------------------------------------------------*/
	Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(TxPower));
	lteHelper->SetAttribute("PathlossModel",StringValue(pathLossModel));
	lteHelper->SetPathlossModelAttribute("Frequency", DoubleValue(frequency));
	lteHelper->SetPathlossModelAttribute("InternalWallLoss", DoubleValue(internalWallLoss));
	lteHelper->SetPathlossModelAttribute("ShadowSigmaIndoor", DoubleValue(shadow));
	lteHelper->SetPathlossModelAttribute("ShadowSigmaOutdoor",DoubleValue(7.0));
	lteHelper->SetPathlossModelAttribute("ShadowSigmaExtWalls",DoubleValue(5.0));
	lteHelper->SetEnbDeviceAttribute("DlEarfcn", UintegerValue(dl_earfcn));
	lteHelper->SetEnbDeviceAttribute("UlEarfcn", UintegerValue(dl_earfcn+ul_earfcn));
	lteHelper->SetEnbDeviceAttribute("DlBandwidth", UintegerValue(dl_bw)); //Value is equivalent to Resource Blocks
	lteHelper->SetEnbDeviceAttribute("UlBandwidth", UintegerValue(ul_bw)); //Value is equivalent to Resource Blocks
	lteHelper->SetPathlossModelAttribute ("Los2NlosThr", DoubleValue (1e6));
	lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");
	if(TxPower>20)
	{
		lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");
	}
	else
	{
		lteHelper->SetEnbAntennaModelType ("ns3::IsotropicAntennaModel");
	}
	return (lteHelper->InstallEnbDevice(nodes));
}

void printTopology (Ptr<MobilityModel> mob, Vector pos, uint16_t neNbs, uint16_t nHeNbs,uint16_t nWiFi, NodeContainer& eNbsnodes, NodeContainer& HeNbsnodes, NodeContainer& WiFinodes)
{
	for (uint32_t i=0;i<nWiFi;i++){
			if (i<neNbs)
			{
				mob = eNbsnodes.Get(i)->GetObject<MobilityModel>();
				cout << "The position of eNodeB: " << i+1 << " is x: " << mob->GetPosition().x  << " and y: " << mob->GetPosition().y << endl;
			}
			if (i<nHeNbs)
			{
				mob = HeNbsnodes.Get(i)->GetObject<MobilityModel>();
				cout << "The position of HeNodeB: " << i+1 << " is x: " << mob->GetPosition().x  << " and y: " << mob->GetPosition().y << endl;
			}
			mob = WiFinodes.Get(i)->GetObject<MobilityModel>();
			cout << "The position of WiFiAP: " << i+1 << " is x: " << mob->GetPosition().x  << " and y: " << mob->GetPosition().y << endl;
		}
}

void AppsSetup (uint16_t &dlPort, uint16_t &ulPort, uint32_t u, Time startTime, Time endTime, uint16_t stype)
{
  ApplicationContainer cApps, sApps;

  Ptr<Node> ue = UEsnodes.Get(u);

  // Set the default gateway for the UE
  Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
  ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

  ++ulPort;
  ++dlPort;
  PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
  PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
  sApps.Add (dlPacketSinkHelper.Install (UEsnodes.Get(u)));
  sApps.Add (ulPacketSinkHelper.Install (remoteHost));

  UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
  dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(10))); //3
  dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
  if (stype == VOICE)
    dlClient.SetAttribute ("PacketSize", UintegerValue(100)); //18
  else if (stype == VIDEO)
    dlClient.SetAttribute ("PacketSize", UintegerValue(500));
  else if (stype == VOIP)
    dlClient.SetAttribute ("PacketSize", UintegerValue(200));



  if (stype == VOICE) {
    UdpClientHelper ulClient (remoteHostAddr, ulPort);
    ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(packet_interval))); //10
    ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
    ulClient.SetAttribute ("PacketSize", UintegerValue(voice_packet_size)); //18,80
    cApps.Add (ulClient.Install (ue));
  }
  else if (stype == VIDEO)
  {
	UdpClientHelper ulClient (remoteHostAddr, ulPort);
	ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(packet_interval))); //10
	ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
	ulClient.SetAttribute ("PacketSize", UintegerValue(400)); //100
    cApps.Add (ulClient.Install (ue));
  }
  else if (stype == VOIP)
  {
	UdpClientHelper ulClient (remoteHostAddr, ulPort);
    ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(packet_interval))); //10
    ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
    ulClient.SetAttribute ("PacketSize", UintegerValue(voip_packet_size)); //200
    cApps.Add (ulClient.Install (ue));
  }

  cApps.Add (dlClient.Install (remoteHost));

  //std::cout << "STARTTIME :::: " << startTime << " ENDTIME :::: " << endTime << std::endl;

  sApps.Start (startTime);
  cApps.Start (startTime);
  sApps.Stop (endTime);
  cApps.Stop (endTime);
}

/* TODO
RngSeedManager::SetSeed (3);
*/
void TrafficTimeline (Ptr<UniformRandomVariable> sTypes, uint16_t i, double simTimeNS)
{
	int stype;
	double startTime, endTime;
	endTime = 0.0;
	string serv;

	while(1)
	{
		stype = (int)sTypes->GetValue ();

		if(stype == VOICE)
		{
		  startTime = endTime + 0.0001;
		  endTime = startTime + std::abs(endTimeVoice->GetValue ());
		}
		else if (stype == VIDEO)
		{
		  startTime = endTime + 0.0001;
		  endTime = startTime + std::abs(endTimeVideo->GetValue ());
		}
		else if (stype == VOIP)
		{

		 startTime = endTime + 0.0001;
		 endTime = startTime + std::abs(endTimeVoip->GetValue ());
		}
		if (endTime >= simTimeNS)
		  break;

		if(stype == VOICE)
		  serv = "VOICE";
		else if(stype == VIDEO)
		  serv = "VIDEO";
		else if(stype == VOIP)
		  serv = "VOIP";
		cout << "Service Type : \t" << serv << "\tStart Time :\t" << startTime << "\tEnd Time :\t" << endTime << "\tduration :\t" <<  endl;
		AppsSetup (dlPort, ulPort, i, Seconds(startTime), Seconds(endTime), stype);
	}
}

void addX2intf ()
{
	lteHelper->AddX2Interface(eNbsnodes);
	lteHelper->AddX2Interface(HeNbsnodes);
	for(int i=0;i<neNbs;i++)
	{
		for(int j=0;j<nHeNbs;j++)
		{
			lteHelper->AddX2Interface(HeNbsnodes.Get(j),eNbsnodes.Get(i));
			cout<< "Adding X2 interface for HeNB = "<<j<<" and eNB = "<<i<<endl;
		}
	}
	ueLteDevs = lteHelper->InstallUeDevice(UEsnodes);
}

void createInternet()
{
	remoteHost = remoteHostContainer.Get(0);/*later we will need to use remoteHost as a ptr*/
	internet.Install(remoteHostContainer);
	cout << "The Remote Host,means the Internet,for lte created with success!!" << endl;
	/*Create Internet Protocol Stack-Connections-Attributes*/
	PointToPointHelper p2ph;
	p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("10Gb/s"))); // 1mb/s
	p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
	p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.5)));
	NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
	address.SetBase("192.168.1.0", "255.255.255.0");
	internetIpIfaces = address.Assign(internetDevices);
	remoteHostAddr = internetIpIfaces.GetAddress(1); /* interface 0 is localhost, 1 is the p2p device */
	Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
	remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),Ipv4Mask("255.0.0.0"),1);
	internet.Install(UEsnodes);
	ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));
	cout << "The Internet Protocol Stack-Connections-Attributes for lte nodes created with success!!" << endl;
}

void assignIP ()
{
	for (uint32_t u = 0; u < UEsnodes.GetN(); ++u)
	{
		Ptr<Node> ueNode = UEsnodes.Get(u);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
		ueStaticRouting->SetDefaultRoute(
		epcHelper->GetUeDefaultGatewayAddress(),1);
	}
	lteHelper->Attach(ueLteDevs);
}

void NotifyConnectionEstablishedUe(std::string context, uint64_t imsi,uint16_t cellid, uint16_t rnti)
{
	cout << context << " UE IMSI " << imsi << ": connected to CellId " << cellid << " with RNTI " << rnti << endl;
	if(imsi<=(uint64_t)nUEs)
	{
		Ues_per_Cell[0]++;
	}
}

void NotifyHandoverStartUe(std::string context, uint64_t imsi, uint16_t cellid,uint16_t rnti, uint16_t targetCellId)
{
	cout << context << " UE IMSI " << imsi << ": previously connected to CellId " << cellid << " with RNTI "<< rnti << ", doing handover to CellId " << targetCellId <<endl;
}

void NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid,uint16_t rnti)
{
	cout << context << " UE IMSI " << imsi << ": successful handover to CellId " << cellid << " with RNTI " << rnti << endl;
}

void NotifyConnectionEstablishedEnb(std::string context, uint64_t imsi,uint16_t cellid, uint16_t rnti)
{
	eNbLteDevice = alleNbs.Get(cellid-1)->GetObject<LteEnbNetDevice>();
	std::cout << context << " eNB CellId " <<  cellid << " with dl bandwidht :  " << (int)eNbLteDevice->GetDlBandwidth() << " with dl earfcn :  " << (int)eNbLteDevice->GetDlEarfcn()
			<< ": successful connection of UE with IMSI " << imsi << " RNTI "
			<< rnti << std::endl;
}

void NotifyHandoverStartEnb(std::string context, uint64_t imsi, uint16_t cellid,uint16_t rnti, uint16_t targetCellId)
{
	cout << context << " eNB CellId " << cellid << ": start handover of UE with IMSI " << imsi << " RNTI " << rnti << " to CellId " << targetCellId << std::endl;
}

void NotifyHandoverEndOkEnb(std::string context, uint64_t imsi, uint16_t cellid,uint16_t rnti)
{
	eNbLteDevice = alleNbs.Get(cellid-1)->GetObject<LteEnbNetDevice>();
	std::cout << context << " eNB CellId " << cellid
			<< ": completed handover of UE with IMSI " << imsi << " RNTI "
			<< rnti << " with dl bandwidht :  " << (int)eNbLteDevice->GetDlBandwidth() << " with dl earfcn :  " << (int)eNbLteDevice->GetDlEarfcn() << std::endl;

}

/*void ReportUeMeasurementsCallback(std::string path, uint16_t rnti,uint16_t cellId, double rsrp, double rsrq, bool servingCell) {
	if (servingCell == 1) {
		if (counter <= nUEs)
		{
			cout << "The current time is : " << Simulator::Now().GetSeconds() << endl;
			RSRPMes[counter] = rsrp;
			counter++;
		} else {
			counter = 0;
		}
		cout << " " << counter << " " << rnti << " " << rsrp << " " << rsrq << " " << cellId << " " << servingCell;
		cout << "----------------------------------------------------------------------" << endl;
	}

}*/

void callBackandStats()
{
	/*CallBacks*/
		//Config::Connect("/NodeList/*/DeviceList/*/LteUePhy/ReportUeMeasurements",MakeCallback(&ReportUeMeasurementsCallback));

		// connect custom trace sinks for RRC connection establishment and handover notification
		Config::Connect("/NodeList/*/DeviceList/*/ ",
				MakeCallback(&NotifyConnectionEstablishedEnb));
		Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
				MakeCallback(&NotifyConnectionEstablishedUe));
		Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
				MakeCallback(&NotifyHandoverStartEnb));
		Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
				MakeCallback(&NotifyHandoverStartUe));
		Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
				MakeCallback(&NotifyHandoverEndOkEnb));
		Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
				MakeCallback(&NotifyHandoverEndOkUe));

		// This callback trigges the handover algorithm
		//Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/RecvMeasurementReport",MakeCallback(&RecvMeasurementReportCallback));
		//Config::Connect("/NodeList/*/DeviceList/*/LteUePhy/ReportUeMeasurements",MakeCallback(&ReportUeMeasurementsCallback));

		/*Traces files*/

		lteHelper->EnablePdcpTraces();
		lteHelper->GetPdcpStats()->SetUlPdcpOutputFilename(ul_pdcp_flname);
		lteHelper->GetPdcpStats()->SetDlPdcpOutputFilename(dl_pdcp_flname);
}
void Initialise_Arrays()
{
	for(uint32_t i=0;i<nUEs_Array;i++)
	{
		CellMap[i] = 0;
		for(uint16_t kk = 0; kk<3;kk++)
		{
			ueIds[i].txBytes[kk] = 0;
			ueIds[i].txPackets[kk] = 0;
			ueIds[i].rxBytes[kk]= 0;
			ueIds[i].rxPackets[kk] = 0;
			ueIds[i].LostPackets[kk] = 0;
		}
	}
	Ues_per_Cell = new uint32_t[3];
	ueIds = new ueInformation[nUEs_Array];
	CellMap = new int [nUEs_Array];
	cout << "Initialization of CellMap array finish with success"<< endl;
}
#endif
