/* A file to hold auxiliary functions to the main simulations. */

#ifndef FUNCS_H
#define FUNCS_H

#include <vector>
#include <cmath>
#include <ns3/globalvars.h>
#include "ns3/random-variable-stream.h"

#define VOICE 0
#define WEBDATA 1
#define STREAMDATA 2
#define IDLE 3

using namespace ns3;
using namespace std;

/*********************** Utility Funtions ************************/

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
}

void
PrintGnuplottableUeListToFile (std::string filename)
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

NetDeviceContainer accessNetworkSetUp(const NodeContainer& nodes, double TxPower, std::string pathLossModel, double frequency, double internalWallLoss, double shadow, double dl_earfcn, double ul_earfcn, double dl_bw, double ul_bw)
{
	/*----------------------------------eNBs Declaration----------------------------------------------------------*/
	Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(TxPower));
	lteHelper->SetAttribute("PathlossModel",StringValue(pathLossModel));
	lteHelper->SetPathlossModelAttribute("Frequency", DoubleValue(frequency));
	lteHelper->SetPathlossModelAttribute("InternalWallLoss", DoubleValue(internalWallLoss));
	lteHelper->SetPathlossModelAttribute("ShadowSigmaIndoor", DoubleValue(shadow));
	lteHelper->SetEnbDeviceAttribute("DlEarfcn", UintegerValue(dl_earfcn));
	lteHelper->SetEnbDeviceAttribute("UlEarfcn", UintegerValue(dl_earfcn+ul_earfcn));
	lteHelper->SetEnbDeviceAttribute("DlBandwidth", UintegerValue(dl_bw)); //Value is equivalent to Resource Blocks
	lteHelper->SetEnbDeviceAttribute("UlBandwidth", UintegerValue(ul_bw)); //Value is equivalent to Resource Blocks
	return (lteHelper->InstallEnbDevice(nodes));
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
/* TODO
RngSeedManager::SetSeed (3);
*/
#endif
