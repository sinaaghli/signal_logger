#include <stdio.h>
#include <string>
#include <Node/Node.h>

#include "FtdiDriver.h"
#include <glog/logging.h>
#include <cstring>
#include <pangolin/pangolin.h>

DEFINE_string(portname,"cu.usbserial-AL00EQVR","Define usb-serial port name");

#define DEBUG   1

FtdiDriver  g_FtdiDriver;

int main( int argc, char** argv )
{
  google::SetUsageMessage("to specify usb-serial port name use:"
                          "--portname=<device_name> ");
  google::ParseCommandLineFlags(&argc,&argv,true);
  google::InitGoogleLogging(argv[0]);

  std::cout << "Poit name is:" << FLAGS_portname.c_str() << std::endl;
  // Setup GUI
  pangolin::CreateWindowAndBind("Main",640,480);
  pangolin::DataLog log;
  std::vector<std::string> labels;
  labels.push_back(std::string("PH1"));
  labels.push_back(std::string("PH2"));
  labels.push_back(std::string("PH3"));
  log.SetLabels(labels);
  const double tinc = 0.01;
  // OpenGL 'view' of data. We might have many views of the same data.
  pangolin::Plotter plotter(&log,0,4*M_PI/tinc,-50,50,M_PI/(4*tinc),0.5);
  plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
  plotter.Track("$i");
  pangolin::DisplayBase().AddDisplay(plotter);
  std::cout << "portname is:" << FLAGS_portname.c_str() << std::endl;
  g_FtdiDriver.Connect(FLAGS_portname.c_str());

  while( !pangolin::ShouldQuit() )
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pangolin::FinishFrame();

    SensorPacket state;
    if( g_FtdiDriver.ReadSensorPacket(state) == 0 ){
      printf("Ftdi read returned 0\n");
    }else
    {
      log.Log(state.ph1,state.ph2,state.ph3);
      if(DEBUG){
        printf("ph1: %d  ph2: %d  ph3: %d \n",state.ph1,state.ph2,state.ph3);
        log.Log(state.ph1,state.ph2,state.ph3);
      }
    }

     // Sleep
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }

  return 0;
}

