#include <NmraDcc.h>

// This Example shows how to use the library as a DCC Accessory Decoder or a DCC Signalling Decoder
// It responds to both the normal DCC Turnout Control packets and the newer DCC Signal Aspect packets 
// You can also print every DCC packet by uncommenting the "#define NOTIFY_DCC_MSG" line below

NmraDcc  Dcc ;
DCC_MSG  Packet ;

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};

const byte            VER_MAJOR     =                2; // Major version in CV 7
const byte            VER_MINOR     =                1; // Minor version in CV 112    
const byte            DCC_PIN       =                2; // DCC input pin.
const int             ACK_PIN       =               A1; // CV acknowledge pin.
const byte            CV_VERSION    =                7; // Decoder version.
const byte            CV_MANUF      =                8; // Manufacturer ID.
const byte            CV_MANUF_01   =              112; // Manufacturer Unique 01.
const byte            MANUF_ID      =       MAN_ID_DIY; // Manufacturer ID in CV 8.
const byte            DECODER_ADDR  =                3; // Decoder address.
const byte            SP_ESTOP      =                0; // Emergency stop speed.
const byte            SP_STOP       =                1; // Stop speed value.
const unsigned long   DELAY_TIME    =               50; // Delay time in ms.

CVPair FactoryDefaultCVs [] =
{
  // The CV Below defines the Short DCC Address
  {CV_MULTIFUNCTION_PRIMARY_ADDRESS, DECODER_ADDR},         // Short address.

  // Reload these just in case they are writeen by accident.
  {CV_VERSION, VER_MAJOR},                                  // Decoder version.
  {CV_MANUF,   MANUF_ID },                                  // Manufacturer ID.

  // These two CVs define the Long DCC Address
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB, 0},               // Extended address MSB.
  {CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB, DECODER_ADDR},    // Extended address LSB.

  {CV_29_CONFIG,                         CV29_F0_LOCATION}, // Short Address 28/128 Speed Steps
  {CV_MANUF_01,                          VER_MINOR},        // Minor decoder version.
};

uint8_t FactoryDefaultCVIndex = 0;

void notifyCVResetFactoryDefault()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset 
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);
};

const int DccAckPin = 3 ;

// This function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read 
void notifyCVAck(void)
{
  Serial.println("notifyCVAck not supported") ;
  
  digitalWrite( DccAckPin, HIGH );
  delay( 6 );  
  digitalWrite( DccAckPin, LOW );
}

// Uncomment the #define below to print all Speed Packets
#define NOTIFY_DCC_SPEED
#ifdef  NOTIFY_DCC_SPEED
void notifyDccSpeed( uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps )
{

  

    Serial.print("notifyDccSpeed: Addr: ");
    Serial.print(Addr,DEC);
    Serial.print( (AddrType == DCC_ADDR_SHORT) ? "-S" : "-L" );
    Serial.print(" Speed: ");
    Serial.print(Speed,DEC);
    Serial.print(" Steps: ");
    Serial.print(SpeedSteps,DEC);
    Serial.print(" Dir: ");
    Serial.println( (Dir == DCC_DIR_FWD) ? "Forward" : "Reverse" );

};
#endif // NOTIFY_DCC_SPEED


// Uncomment to print all DCC Packets
//#define NOTIFY_DCC_MSG
#ifdef  NOTIFY_DCC_MSG
void notifyDccMsg( DCC_MSG * Msg)
{
  Serial.print("notifyDccMsg: ") ;
  for(uint8_t i = 0; i < Msg->Size; i++)
  {
    Serial.print(Msg->Data[i], HEX);
    Serial.write(' ');
  }
  Serial.println();
}
#endif




// This function is called whenever a DCC Signal Aspect Packet is received
void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
{
  Serial.print("notifyDccSigOutputState: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(',');
  Serial.println(State, HEX) ;
}

void setup()
{
  Serial.begin(115200);
  
  // Configure the DCC CV Programing ACK pin for an output
  pinMode( DccAckPin, OUTPUT );
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("NMRA DCC Example 1");
  
  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
  Dcc.pin(0, 2, 1);
  
  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, FLAGS_MY_ADDRESS_ONLY, 0 );

  Serial.println("Init Done");
}

void loop()
{
  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();
  
  if( FactoryDefaultCVIndex && Dcc.isSetCVReady())
  {
    FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array 
    Dcc.setCV( FactoryDefaultCVs[FactoryDefaultCVIndex].CV, FactoryDefaultCVs[FactoryDefaultCVIndex].Value);
  }
}
