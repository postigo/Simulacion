/*
*       PyS   -   Practica8
*     Maria
*     Ramon
*     Postigo
*/
#include <ns3/object.h>
#include <ns3/log.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include "ns3/radio-bearer-stats-calculator.h"
#include <ns3/constant-position-mobility-model.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include <ns3/netanim-module.h>
#include "../src/point-to-point/helper/point-to-point-helper.h"
#include "../build/ns3/ptr.h"
#include "../build/ns3/animation-interface.h"
#include "../build/ns3/simulator.h"
#include "ns3/gnuplot.h"

#include <ns3/lte-enb-phy.h>
#include <ns3/lte-ue-phy.h>

#include "ns3/string.h"
#include "ns3/double.h"
#include <ns3/boolean.h>
#include <ns3/enum.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"

#include "Observador.h"

// Declare namespace 
using namespace ns3;
using std::vector;

NS_LOG_COMPONENT_DEFINE("myX2-MultiUes5-TrialHandover");


/*       Funcion que monitoriza el flujo de los UE             */
void MonitorizadorFlujo (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> flowMon)
  {	
    flowMon->CheckForLostPackets(); 
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
    
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
      {	
	// A tuple: Source-ip, destination-ip, protocol, source-port, destination-port
	Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow (stats->first);
	NS_LOG_INFO("Flow ID: " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress);
	NS_LOG_INFO("Tx Packets = " << stats->second.txPackets);
	NS_LOG_INFO("Rx Packets = " << stats->second.rxPackets);
	NS_LOG_INFO("Duration: "<<stats->second.timeLastRxPacket.GetSeconds() - stats->second.timeFirstTxPacket.GetSeconds());
	NS_LOG_INFO("Last Received Packet: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds");
	NS_LOG_INFO("------------------------------------------");
      }	
    Simulator::Schedule(Seconds(1),&MonitorizadorFlujo, fmhelper, flowMon);
    
    // Write results to an XML file
    flowMon->SerializeToXmlFile ("MonitorizadorFlujo.xml", true, true);
  }


double RealizaSimulacion(double simTime, double posicion[20], double posicion_nodo,uint16_t n_enb) {
  NS_LOG_FUNCTION("Entramos en el método RealizaSimulacion.");

  
  uint16_t   n_ues = 1;	
  uint16_t bearersPorUE = 1;
  //double velocidad = 2*distancia_Enbs/20;	// velocidad del UE en m/s
    


  // Creamos el lteHelper y epcHelper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
	
  // Establecemos el modelo de perdidas
  //lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::FriisPropagationLossModel"));
    
  // Se establece el modelo de Fading
  //lteHelper->SetAttribute ("FadingModel", StringValue ("ns3::TraceFadingLossModel"));
  //lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EVA_60kmph.fad"));
  //lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
  //lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_ETU_3kmph.fad"));
    
  /*
  // These parameters have to setted only in case of the trace format differs from the standard one, that is
  // - 10 seconds length trace
  // - 10,000 samples
  // - 0.5 seconds for window size
  // - 100 RB
  lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
  lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
  lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
  lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));
  */


  /* 
  * Tres formas de hacer handover:
  *          - A2A4: traspaso en funcion de las medidas del algoritmo RSRQ
  *          - A3: realiza el traspaso en funcion de la potencia de la senial
  *          - Por defecto: no gestiona el traspaso automatico
  */	
  // Handover basado en A2, A4
  lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
  lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold", UintegerValue (30));
  lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset", UintegerValue (1));
	
  /* Handover basado en A3
    
   lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
   lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis", DoubleValue (3.0));
   lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger", TimeValue (MilliSeconds (256)));
   */

   //NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  
	
  // Guardamos la pasarela a internet
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
	
  // Creamos un host Remoto
  NodeContainer hostRemotoContainer;
  hostRemotoContainer.Create (1);
  Ptr<Node> hostRemoto = hostRemotoContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (hostRemotoContainer);
	
  // Ponemos el servidor remoto en Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));	
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));		
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));	
  NetDeviceContainer internetDevices = p2ph.Install (pgw, hostRemoto);	// Se conecta al pgw
  // Se pone la direccion de internet
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);	
  // Guardamos la direccion del servidor
  Ipv4Address hostRemotoAddr = internetIpIfaces.GetAddress (1);
	
  // Se establecen las rutas para internet
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> hostRemotoStaticRouting = ipv4RoutingHelper.GetStaticRouting (hostRemoto->GetObject<Ipv4> ());
  hostRemotoStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  
  // Creamos los nodos ue y enb
  NodeContainer nodosUE;
  NodeContainer nodosENB;
  nodosUE.Create (n_ues);
  nodosENB.Create (n_enb);

	
  /**  Establecemos el modelo de mobilidad   
  *
  *            (UE1)	      (UE2)          
  *                       
  *    (UE0)   ENB0           ENB1  
  *                <--d_Enb-->          
  *
  **/


  MobilityHelper ueMobility;
  MobilityHelper henbMobility;
	
  /*
   * Modelo de movilidad para los UEs:
   *            - UE0: se mueve a velocidad constante
   *            - Resto de UEs: estan quietos
   */
  ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  for (uint16_t i = 0; i <     n_ues; i++)
    {
      ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      ueMobility.Install (nodosUE.Get (i));
      nodosUE.Get (i)->GetObject<MobilityModel> ()->SetPosition (Vector (  posicion_nodo , 50, 0));
    }
  
  /*
   * Modelo de movilidad para los ENB: todos son fijos
   */
  Ptr<ListPositionAllocator> HenbPositionAlloc = CreateObject<ListPositionAllocator> ();

  for (uint16_t i = 0; i <  n_enb; i++)
    {
      Vector henbPosition (  posicion[i], 0, 0); // La primera antena estara en (0,0,0)
      HenbPositionAlloc->Add (henbPosition);
    }
  henbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  henbMobility.SetPositionAllocator (HenbPositionAlloc);
  henbMobility.Install (nodosENB);
  
 





 // Se instalan los nodos en el LTEHelper
  NetDeviceContainer henbLteDevs = lteHelper->InstallEnbDevice (nodosENB);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (nodosUE);

/*
  // Set eNB's transmission power
  // In my simulation, I want to add a eNB, but the problem is that ns-3 do not support for small cells like femtocell, picocell, etc.
  // So, I assume that an eNBs and a HeNBs is similar to each other in view of Phy regarless of transmission power.
  Ptr<LteEnbNetDevice> enbLteNetDevs = henbLteDevs.Get (0)->GetObject<LteEnbNetDevice> ();
  Ptr<LteEnbPhy> enbPhy = enbLteNetDevs->GetPhy();
  enbPhy->SetAttribute ("TxPower", (DoubleValue) (  potenciaEnb0/2));
  enbPhy->SetAttribute ("NoiseFigure", (DoubleValue) (  ruidoEnb0));
*/

	
  // Se establece IP en los UEs
  internet.Install (nodosUE);
  Ipv4InterfaceContainer ueIpInterface;

  // Asignamos la IP automaticamente
  ueIpInterface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  for (uint32_t u = 0; u < nodosUE.GetN (); ++u)  
    {
      Ptr<Node> ueNode = nodosUE.Get (u);
      // Ponemos el gateway por defecto
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
	
  // Se conectan los UE al ENB0
  //lteHelper->Attach (ueLteDevs.Get (0), henbLteDevs.Get (1));
  for (uint16_t i = 0; i <  n_ues; i++)	
    {
      		lteHelper->Attach (ueLteDevs.Get (i), henbLteDevs.Get (n_enb-1));
    }
	
  NS_LOG_LOGIC ("Establecemos las aplicaciones en los UE");
	
  // Downlink port and Uplink port
  uint16_t dlPuerto = 10000;
  uint16_t ulPuerto = 20000;
	
  /* 
   * randomize a bit start times to avoid simulation artefacts
   * (e.g., buffer overflows due to packet transmissions happening exactly at the same time)
   */ 
  Ptr<UniformRandomVariable> startSimTimeSeconds = CreateObject<UniformRandomVariable> ();
  startSimTimeSeconds->SetAttribute ("Min", DoubleValue (0));		// 0 second
  startSimTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));	// 0.010 second
	
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;

  for (uint32_t u = 0; u <     n_ues; ++u)
    {
      Ptr<Node> ue = nodosUE.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      for (uint32_t b = 0; b <   bearersPorUE; ++b)
        {
          ++dlPuerto;
          ++ulPuerto;
					
	  // Config UDP Downlink
          NS_LOG_LOGIC ("Installing UDP Downlink Application(s) for UE " << u);

	  BulkSendHelper dlClientHelper ("ns3::TcpSocketFactory",          InetSocketAddress(ueIpInterface.GetAddress (u), dlPuerto));
          clientApps.Add (dlClientHelper.Install (hostRemoto));
          PacketSinkHelper dlPacketSinkHelper ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), dlPuerto));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          NS_LOG_INFO("Configuradas aplicaciones de tráfico descendente.");


	  // Config UDP Uplink
          BulkSendHelper ulClientHelper ("ns3::TcpSocketFactory",InetSocketAddress(hostRemotoAddr, ulPuerto));
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), ulPuerto++));
          serverApps.Add (ulPacketSinkHelper.Install (hostRemoto));

          NS_LOG_INFO("Configuradas aplicaciones de tráfico ascendente.");
		
	
	  // Filter downlink local port
          Ptr<EpcTft> tft = Create<EpcTft> ();
          EpcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPuerto;
          dlpf.localPortEnd = dlPuerto;
          tft->Add (dlpf);
	  // Filter uplink remote port
          EpcTft::PacketFilter ulpf;
          ulpf.remotePortStart = ulPuerto;
          ulpf.remotePortEnd = ulPuerto;
          tft->Add (ulpf);
	  /*
	   * Activate bearer to transfer TCP-based video application, and UEs are a kind of NGBR UEs.
	   * NGBR: None Guaranteed Bit Rate
	   */
          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

          Time startSimTime = Seconds (startSimTimeSeconds->GetValue ());
          serverApps.Start (startSimTime);
          clientApps.Start (startSimTime);
    }
  }
		
  // Se aniade la interfaz X2
  lteHelper->AddX2Interface (nodosENB);
  // Habilitamos las trazas
  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();
  lteHelper->EnablePdcpTraces ();
  
  // Establecemos las estadisticas
  Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
	

  //*  Da violación de segmento no se por que
  Observador nodos_obs;
  if (bearersPorUE >= 1)
  {
    //  void CapturaTrazas(Ptr<Application>, Ptr<Application>, Ptr<LteUeNetDevice> ,Ptr<NetDevice>, Ptr<NetDevice>);
    nodos_obs.CapturaTrazas(clientApps.Get(0), serverApps.Get(0), DynamicCast<LteUeNetDevice> (ueLteDevs.Get(0)),internetDevices.Get (0), internetDevices.Get (1)); 
  }


 
  
  
  // Start simulation
  Simulator::Stop (Seconds (  simTime));



    // Suffice to say that enabling the flow monitor is just the matter of replacing the line ns3.Simulator.Run ()
    // Throughput Monitor
  FlowMonitorHelper fmhelper;
  NodeContainer flowMon_nodes;
  flowMon_nodes.Add (nodosUE);
  flowMon_nodes.Add (hostRemotoContainer);
  Ptr<FlowMonitor> monitor;
  //monitor = fmhelper.InstallAll();
  monitor = fmhelper.Install (flowMon_nodes);
  MonitorizadorFlujo (&fmhelper, monitor);
  
  Simulator::Run ();


  // Imprime el retraso
  for (int j = 0;j<1;j++) {
        NS_LOG_INFO ("  ----  Ue: " << j << " ----- " );
                
        for (int i = 3; i<5; i++) {
                uint64_t imsi = ueLteDevs.Get (j)->GetObject<LteUeNetDevice> ()->GetImsi ();
                std::vector< double > stats = lteHelper->GetPdcpStats()->GetDlDelayStats(imsi,i); // imsi, lcid
  
                NS_LOG_INFO ("DL:" << i << ":  avg Dl delay (in seconds) of target UE = " << stats[0] / 1.0e+9 );
                NS_LOG_INFO ("DL:" << i << ":  std dev Dl delay (in seconds) of target UE = " << stats[1] / 1.0e+9 );  
                NS_LOG_INFO ("DL:" << i << ":  min Dl delay (in seconds) of target UE = " << stats[2] / 1.0e+9 );
                NS_LOG_INFO ("DL:" << i << ":  max Dl delay (in seconds) of target UE = " << stats[3] / 1.0e+9 );
                NS_LOG_INFO ("DL:" << i << ":  Paquetes enviados: " <<  lteHelper->GetPdcpStats()->GetDlTxPackets (imsi,i)  << " -- Paquetes recibidos: " << lteHelper->GetPdcpStats()->GetDlRxPackets (imsi,i) );
                NS_LOG_INFO ("UL:" << i << ":  Paquetes enviados: " <<  lteHelper->GetPdcpStats()->GetUlTxPackets (imsi,i)  << " -- Paquetes recibidos: " << lteHelper->GetPdcpStats()->GetUlRxPackets (imsi,i) );

        }
  }
  
  double resultado_sim = nodos_obs.DevuelvePorcentajeCorrectos();
  //  double resultado_sim = 0;

	
  // End simulation
  Simulator::Destroy ();

  return resultado_sim;

}



/* 
 * Se simulan 2 EnB fijos, 2 UE fijo y 1 UE que se mueve a velocidad constante
 * Se usará un algoritmo de handover automatico para el UE movil 
 * También se priorizara el trafico del UE movil frente al UE fijo
 *
 * Se investigara la distancia optima entre las antenas y donde se pierden mas paquetes
 * Se compararan los resultados del UE fijo y el UE movil
 */


int
main (int argc, char *argv[])
{ 

  /**  Variables  ***/
    double distancia_Enbs = 100;	// Distancia en metros
    double t_simulacion = 20.0;
    //double simTime = (double) ((  n_enb + 1) * distancia_Enbs / velocidad); // Tiempo de simulacion
    
    double potenciaEnb = 20;
    double ruidoEnb = 8;
    double potenciaUe = 20;
    double ruidoUe = 8;
    double posicion_nodo = 20;
     
    uint32_t iteraciones = 3;
    uint16_t n_enb = 1;
    double posicion[20];

  /** Configuracion por defecto  **/

  /* Antes de procesar los argumentos pasados por línea de comandos, vamos a modificar
  algunos atributos por defecto de clases que se utilizarán en el código para que tengan
  valores razonables. Al hacerlo antes, el usuario podrá cambiarlo si lo desea (por línea
  de comandos, por ejemplo). */

  // Usaremos el modelo real para la señalización RRC (de gestión de recursos radio).

  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (false));

  // Usamos un modelo exponencial para las pérdidas en propagación en el radioenlace.

  Config::SetDefault ("ns3::LteHelper::PathlossModel", StringValue ("ns3::LogDistancePropagationLossModel"));

  // Se modifican algunos atributos para las antenas como la potencia y el ruido
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (false));
  Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue (potenciaEnb));
  Config::SetDefault("ns3::LteEnbPhy::NoiseFigure", DoubleValue (ruidoEnb));
  Config::SetDefault("ns3::LteUePhy::TxPower", DoubleValue (potenciaUe));
  Config::SetDefault("ns3::LteUePhy::NoiseFigure", DoubleValue (ruidoUe));
	  
  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("distancia_Enbs", "Distance between two adjacent HeNBs", distancia_Enbs);
  cmd.AddValue ("t_simulacion", "Total duration of the simulation", t_simulacion);
  cmd.Parse (argc, argv);
  
  NS_LOG_INFO("Fin de tratamiento de variables de entorno.");

  //   Definicion de Acumuladores
   Average<double> ac_porc_simulacion;

  //   DEFINICION GRAFICAS
  Gnuplot plot_porc;
  plot_porc.SetTitle ("P8  - marvalcam1 josposagu ramperher");
  plot_porc.SetLegend ("distancia a cero", "Porcentaje de paquetes correctos"); 
  
  Gnuplot2dDataset g_porc;
  g_porc.SetTitle ("Porcentaje de paquetes de la App correctos");
  g_porc.SetStyle (Gnuplot2dDataset::POINTS);
  g_porc.SetErrorBars (Gnuplot2dDataset::Y);

  Gnuplot plot_num_enb;
  plot_num_enb.SetTitle ("P8  - marvalcam1 josposagu ramperher");
  plot_num_enb.SetLegend ("distancia a cero", "Numero de enBs"); 
  
  Gnuplot2dDataset g_enb;
  g_enb.SetTitle ("Numero de enBs que se aniaden por la posicion");
  g_enb.SetStyle (Gnuplot2dDataset::POINTS);
  g_enb.SetErrorBars (Gnuplot2dDataset::Y);

  // Simulacion
  posicion[(n_enb-1)]=0;
  g_enb.Add(posicion[n_enb-1], (n_enb),0);

  for (uint32_t i = 2; posicion_nodo*i < 4000 ; i++) {
    NS_LOG_UNCOND("distancia: " << posicion_nodo*i  <<  " - Numero de EnB: " << n_enb); 
    double resultado;
    for (uint32_t j = 0;j<iteraciones;j++)
	{
    		resultado = RealizaSimulacion(t_simulacion, posicion, posicion_nodo*i, n_enb);
  	        ac_porc_simulacion.Update(resultado);
                NS_LOG_UNCOND("     - Porcentaje correctos: " << resultado << "   -   iteracion: " << j);
                
	}

    NS_LOG_UNCOND("    FIN MEDIA: Porcentaje de paq correctos " << ac_porc_simulacion.Avg() << "%");
 
    double inter_conf =  2.9200*ac_porc_simulacion.Stddev()/iteraciones;
    g_porc.Add( posicion_nodo*i , ac_porc_simulacion.Avg(),inter_conf);
          
    ac_porc_simulacion.Reset ();


    if (resultado < 45.0 )
	{
           posicion[n_enb]=(posicion_nodo*i) + ( (posicion_nodo*i)-posicion[(n_enb-1)] ); // Posicion de la nueva antena
           g_enb.Add(posicion[n_enb], (n_enb+1),0);
	   n_enb++;
           i=i+1; // Para se acerque a la nueva antena y vuelva a tener un bajo procentaje 
	}
    }

  // Exportar las graficas
  plot_porc.AddDataset (g_porc);
  plot_num_enb.AddDataset (g_enb);
 
  std::ofstream fichero1("p7-porc-grupo.plt");
  plot_porc.GenerateOutput (fichero1);
  fichero1 << "pause -1" << std::endl;
  fichero1.close();
  
  std::ofstream fichero2("p7-enb-grupo.plt");
  plot_num_enb.GenerateOutput (fichero2);
  fichero2 << "pause -1" << std::endl;
  fichero2.close();  
 
  
  std::cout << "The simulation finished!" << std::endl; 
  std::cout << "" << std::endl;
	
  return 0;
}

