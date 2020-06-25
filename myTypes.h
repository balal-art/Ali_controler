#ifndef Global_h
#define Global_h

typedef enum Out_State : byte
{
  OUT_OFF,
  OUT_ON,
  OUT_TURNOFF,
  OUT_TURNON,
};
//
typedef enum Controled_Method : byte
{
  CM_ONOFF,      // Bang-Bang
  CM_PID,      // PID
  CM_SLOWBANG, //
  CM_DEADTIME, //
};

typedef enum Controled_Type : byte
{
  MOSFEET,
  MOSFEET_DRIVE,
  RELAYNC,
  RELAYNC_DRIVE,
  RELAYNO,
  RELAYNO_DRIVE,
};
#endif
