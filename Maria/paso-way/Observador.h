
/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

/*  PRÁCTICA 8. PLANIFICACIÓN Y SIMULACIÓN DE REDES
*     Observador.h
*       -Ramón Pérez Hernández
*       -José Manuel Postigo Aguilar
*       -María Valero Campaña
*/

#include "ns3/application.h"
#include "ns3/packet.h"
#include "ns3/address.h"
#include "ns3/double.h"

#include "ns3/net-device.h"
#include "ns3/lte-enb-net-device.h"
#include "ns3/lte-ue-net-device.h"
#include "ns3/lte-ue-rrc.h"

using namespace ns3;

class Observador
{
public:

  // Constructor de la clase.
  Observador();

  // Método para resetear los atributos del observador.
  void Inicializa();

  /* Método de gestión de la traza de envío completo de un paquete por la 
  aplicación origen. */
  void GestionaTrazaTxApp(Ptr <const Packet> p);

  /* Método de gestión de la traza de recepción completa de un paquete en la
  aplicación destino. */
  void GestionaTrazaRxApp(Ptr <const Packet> p, const Address & direccion);

  // Método que captura las trazas consideradas.
  void CapturaTrazas(Ptr<Application>, Ptr<Application>, Ptr<LteUeNetDevice> , Ptr<NetDevice>, Ptr<NetDevice>);

  //
  void PacketReceive(Ptr<const Packet> p);
  //
  void PacketSend(Ptr<const Packet> p);
  //
  void NotifyHandoverEnd (uint64_t imsi, uint16_t cellid, uint16_t rnti);
  //
  void NotifyHandoverStart (uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);
  //
  void Conexion( uint64_t imsi, uint16_t cellid, uint16_t rnti);

  // Método que devuelve el porcentaje de paquetes correctos en la aplicación destino.
  double DevuelvePorcentajeCorrectos();

  // Paquetes enviados por la aplicación origen.
  double pkts_enviados;
  
  // Paquetes recibidos por la aplicación destino.
  double pkts_recibidos;

private:

  // Aplicación transmisora en el escenario.
  Ptr<Application> transmisor;

  // Aplicación receptora en el escenario.
  Ptr<Application> receptor;
 
  //
  uint64_t mac_paquetesRecibidos; 
  //
  uint64_t mac_paquetesEnviados;

};



