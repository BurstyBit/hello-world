/**
* @file system-test.c
* @brief CCIMX93 Development board system test
* @author Pieter Kruger
* @date 2025-07-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void WelcomeMessagePrint ( void )
{
  printf("\n=== CCIMX93 Utilities ===\n");
  printf("Welcome to the CCIMX93 utility functions!\n");
}

void SqliteAvailableTest ( void )
{
  int result;

  printf("\n--- Testing SQLite3 ---\n");
  result = system("sqlite3 --version 2>/dev/null");
  if (result == 0)
  {
    printf("✓ SQLite3 is installed and working\n");
  }
  else
  {
    printf("✗ SQLite3 not found or not working\n");
  }
}

void ApacheStatusTest ( void )
{
  int result;

  printf("\n--- Testing Apache2 Status ---\n");
  result = system("systemctl is-active apache2 >/dev/null 2>&1");
  if ( result == 0 )
  {
    printf("✓ Apache2 is running\n");
  }
  else
  {
    printf("✗ Apache2 is not running\n");
  }
}

void WifiStatusTest ( void )
{
  int result;

  printf("\n--- Testing WiFi Status ---\n");
  result = system("iwconfig 2>/dev/null | grep -q 'wlan'");
  if ( result == 0 )
  {
    printf("✓ WiFi interface detected\n");
    system("iwconfig 2>/dev/null | grep 'wlan' | head -2");
  }
  else
  {
    printf("✗ No WiFi interface found\n");
  }
}

int main ( void )
{
  WelcomeMessagePrint();
  SqliteAvailableTest();
  ApacheStatusTest();
  WifiStatusTest();    
  printf("\n=== System Test Complete ===\n");
  
  return 0;
}
