#include <stdio.h>
#include <string>
#include <Node/Node.h>

#include "FtdiDriver.h"
#include "GetPot"
#include <cstring>

bool DEBUG = false;

FtdiDriver  g_FtdiDriver;
node::node  ninja_node;
NinjaCommandMsg cmd;
NinjaStateMsg state_msg;


NinjaStateMsg BuildNinjaStateMsg( const SensorPacket& p )
{
  NinjaStateMsg msg;
  msg.set_acc_x( p.Acc_x );
  msg.set_acc_y( p.Acc_y );
  msg.set_acc_z( p.Acc_z );
  msg.set_gyro_x( p.Gyro_x );
  msg.set_gyro_y( p.Gyro_y );
  msg.set_gyro_z( p.Gyro_z );
  msg.set_mag_x( p.Mag_x );
  msg.set_mag_y( p.Mag_y );
  msg.set_mag_z( p.Mag_z );
  msg.set_enc_lb( p.Enc_LB );
  msg.set_enc_lf( p.Enc_LF );
  msg.set_enc_rb( p.Enc_RB );
  msg.set_enc_rf( p.Enc_RF );
  msg.set_adc_steer( p.ADC_Steer );
  msg.set_adc_lb( p.ADC_LB );
  msg.set_adc_lf( p.ADC_LF );
  msg.set_adc_rb( p.ADC_RB );
  msg.set_adc_rf( p.ADC_RF );

  return msg;
}

int main( int argc, char** argv )
{
  GetPot cl(argc,argv);
  std::string dev = cl.follow("/dev/ttyUSB0","--dev");
  bool bNodeSubscribed = false;

  ninja_node.init("ninja_node");

  printf("Connecting to FTDI com port '%s'...\n", dev.c_str() );
  g_FtdiDriver.Connect(dev);

  // advertise that we will transmit state data
  if( ninja_node.advertise("state") == false ) {
    printf("Error setting publisher.\n");
  }

  for( size_t ii = 0; ; ++ii ) {

    // read from the car's microcontroller
    SensorPacket state,new_state;

    //Check package contents received over usb
    if( g_FtdiDriver.ReadSensorPacket(new_state) == 0 ){
      printf("Ftdi read returned 0\n");
    }else
    {
      memmove(&state,&new_state,sizeof(SensorPacket));
      if(DEBUG){
        printf("*********************************************************\n");
        printf("acc_x: %d  acc_y: %d  acc_z: %d \n",state.Acc_x,state.Acc_y,state.Acc_z);
        printf("Gyro_x: %d  Gyro_y: %d  Gyro_z: %d \n",state.Gyro_x,state.Gyro_y,state.Gyro_z);
        printf("Mag_x: %d  Mag_y: %d  Mag_z: %d \n",state.Mag_x,state.Mag_y,state.Mag_z);
        printf("Enc_lb: %d  Enc_lf: %d  Enc_rb: %d  Enc_rf: %d \n",state.Enc_LB,state.Enc_LF,state.Enc_RB,state.Enc_RF);
        printf("adc_steer: %d  adc_lb: %d  adc_lf: %d  adc_rb: %d  adc_rf: %d \n", state.ADC_Steer,state.ADC_LB,state.ADC_LF,state.ADC_RB,state.ADC_RF);
      }
    }

    // subscribe to the ninja_command topic
    if( !bNodeSubscribed ){
      if( ninja_node.subscribe("commander_node/command") == false ){
        printf("Error subscribing to commander_node/command.\n");
      }
      bNodeSubscribed = true;
    }
    // package and send our state protobuf
    NinjaStateMsg state_msg = BuildNinjaStateMsg( state );
    if ( ninja_node.publish( "state", state_msg ) == false ) {
      printf("Error sending message.\n");
    }

    // let's see what the controller is telling us to do
    NinjaCommandMsg cmd;
    if( ninja_node.receive("commander_node/command", cmd) ){
//      if( DEBUG ){
        printf("Received acc=%.2f and phi=%.2f\n", cmd.speed(), cmd.turnrate() );
//      }
      // relay command to the car
      g_FtdiDriver.SendCommandPacket( (float)cmd.speed(), (float)cmd.turnrate() );
    }else
    {
	bNodeSubscribed = false;
    }

    // Sleep
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}

