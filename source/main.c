/**
* @file main.c
* @brief CCIMX93 Development board Hello World Application
* @author Pieter Kruger
* @date 2025-07-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

void BannerPrint(void)
{
  printf("================================================\n");
  printf("         CCIMX93 Hello World Application       \n");
  printf("================================================\n");
}

void SystemInformationPrint ( void )
{
  struct utsname information;
  time_t currentTime;
  char *timeString;
  
  printf("\n--- System Information ---\n");
  if ( uname(&information) == 0 )
  {
    printf("System: %s\n", information.sysname);
    printf("Node: %s\n", information.nodename);
    printf("Release: %s\n", information.release);
    printf("Version: %s\n", information.version);
    printf("Machine: %s\n", information.machine);
  }
  currentTime = time(NULL);
  timeString = ctime(&currentTime);
  printf("Current time: %s", timeString);
  printf("Process ID: %d\n", getpid());
}

void NetworkInformationPrint ( void )
{
  printf("\n--- Network Information ---\n");
  printf("WiFi AP should be available at: PIET_1234\n");
  printf("Device IP: 192.168.1.93\n");
  printf("DHCP Range: 192.168.1.50 - 192.168.1.200\n");
  printf("Web interface: https://192.168.1.93\n");
}

void UsagePrint ( const char * programName )
{
  printf("\nUsage: %s [options]\n", programName);
  printf("Options:\n");
  printf("  -h, --help     Show this help message\n");
  printf("  -v, --version  Show version information\n");
  printf("  -s, --system   Show system information\n");
  printf("  -n, --network  Show network information\n");
  printf("  -a, --all      Show all information\n");
}

void VersionPrint ( void )
{
  printf("Hello World Application v1.0\n");
  printf("Built for CCIMX93 Development Board\n");
  printf("Compiled on %s at %s\n", __DATE__, __TIME__);
}

int main ( int argc, char * argv[] )
{
  int showSystem = 0;
  int showNetwork = 0;
  int showAll = 0;

  // Parse command line arguments
  for ( int i = 1 ; i < argc ; i++ )
  {
    if ( strcmp(argv[i], "-h") == 0 ||
         strcmp(argv[i], "--help") == 0 )
    {
      BannerPrint();
      UsagePrint(argv[0]);
      return 0;
    }
    else if ( strcmp(argv[i], "-v") == 0 ||
              strcmp(argv[i], "--version") == 0 )
    {
      VersionPrint();
      return 0;
    }
    else if ( strcmp(argv[i], "-s") == 0 ||
              strcmp(argv[i], "--system") == 0 )
    {
      showSystem = 1;
    }
    else if ( strcmp(argv[i], "-n") == 0 ||
              strcmp(argv[i], "--network") == 0 )
    {
      showNetwork = 1;
    }
    else if ( strcmp(argv[i], "-a") == 0 ||
              strcmp(argv[i], "--all") == 0 )
    {
      showAll = 1;
    }
    else
    {
      printf("Unknown option: %s\n", argv[i]);
      UsagePrint(argv[0]);
      return 1;
    }
  }
  BannerPrint();
  printf("Hello from the CCIMX93 Development Board!\n");
  printf("This application was built using Yocto and deployed successfully.\n");
  if ( showAll ||
       showSystem )
  {
    SystemInformationPrint();
  }
  if ( showAll ||
       showNetwork )
  {
    NetworkInformationPrint();
  }
  if ( !showSystem &&
       !showNetwork &&
       !showAll )
  {
    printf("\nRun '%s --help' for more options.\n", argv[0]);
  }
  printf("\n✓ Application completed successfully!\n");

  return 0;
}
