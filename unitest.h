#ifndef UNITEST_H
#define UNITEST_H

#ifndef HAT_03

#define HAT_ID_MCC_134 0x0143

#endif

#ifndef HAT_04

#define HAT_ID_MCC_152 0x0144

#endif

#ifndef HAT_05
enum SourceType
{
    /// Use a local-only source.
    SOURCE_LOCAL    = 0,
    /// Use a local source and set it as master.
    SOURCE_MASTER   = 1,
    /// Use a separate master source.
    SOURCE_SLAVE    = 2
};

enum TriggerMode
{
    /// Start the scan on a rising edge of TRIG.
    TRIG_RISING_EDGE    = 0,
    /// Start the scan on a falling edge of TRIG.
    TRIG_FALLING_EDGE   = 1,
    /// Start the scan any time TRIG is high.
    TRIG_ACTIVE_HIGH    = 2,
    /// Start the scan any time TRIG is low.
    TRIG_ACTIVE_LOW     = 3
};

// Scan status bits

/// A hardware overrun occurred.
#define STATUS_HW_OVERRUN       (0x0001)
/// A scan buffer overrun occurred.
#define STATUS_BUFFER_OVERRUN   (0x0002)
/// The trigger event occurred.
#define STATUS_TRIGGERED        (0x0004)
/// The scan is running (actively acquiring data.)
#define STATUS_RUNNING          (0x0008)



#define HAT_ID_MCC_172 0x0145
#endif

typedef enum
{
    TYPE_UL_INFO = 0,
    TYPE_DEV_INFO = 1,
    TYPE_AI_INFO = 2,
    TYPE_AO_INFO = 3,
    TYPE_DIO_INFO = 4,
    TYPE_CTR_INFO = 5,
    TYPE_TMR_INFO = 6,
    TYPE_DAQI_INFO = 7,
    TYPE_DAQO_INFO = 8,
    TYPE_MEM_INFO = 9
}UtInfoTypes;

typedef enum
{
    FUNC_GROUP_DISC = 0,
    FUNC_GROUP_AIN = 1,
    FUNC_GROUP_AOUT = 2,
    FUNC_GROUP_DIN = 3,
    FUNC_GROUP_DOUT = 4,
    FUNC_GROUP_CTR = 5,
    FUNC_GROUP_MISC = 6,
    FUNC_GROUP_CONFIG = 7,
    FUNC_GROUP_DAQSCAN = 8,
    FUNC_GROUP_STATUS = 9
}UtFunctionGroup;

typedef enum
{
    UL_AIN = 1,
    UL_AINSCAN = 2,
    UL_AIN_LOAD_QUEUE = 3,
    UL_AIN_SET_TRIGGER = 4,
    UL_DAQ_INSCAN = 5,
    UL_TIN = 6
}UtAiFunctions;

typedef enum
{
    UL_AOUT = 1,
    UL_AOUT_SCAN = 2,
    UL_DAQ_OUTSCAN = 3,
    UL_AOUT_SET_TRIGGER = 5
}UtAoFunctions;

typedef enum
{
    UL_D_CONFIG_PORT = 1,
    UL_D_CONFIG_BIT = 2,
    UL_D_IN = 3,
    UL_D_OUT = 4,
    UL_D_BIT_IN = 5,
    UL_D_BIT_OUT = 6,
    UL_D_INT_PORT = 7,
    UL_D_INT_BIT = 8,
    UL_D_INT_WAIT = 9
}UtDioFunctions;

typedef enum
{
    UL_C_SELECT = 1,
    UL_C_IN = 2,
    UL_C_LOAD = 3,
    UL_C_CLEAR = 4,
    UL_C_CONFIG_SCAN = 5,
    UL_C_INSCAN = 6,
    UL_TMR_OUT = 7
}UtCtrFunctions;

typedef enum
{
    UL_GET_INFO = 1,
    UL_GET_CONFIG = 2,
    UL_SET_CONFIG = 3
}UtInfoFunctions;

typedef enum
{
    UL_DISC = 0,
    UL_FLASH_LED = 1,
    UL_GET_ERR_MSG = 2,
    UL_GET_STATUS = 3,
    UL_SCAN_STOP = 4,
    UL_AI_INFO = 5,
    UL_AO_INFO = 6,
    UL_DIO_INFO = 7,
    UL_CTR_INFO = 8,
    UL_TEMP_INFO = 9,
    UL_TEST = 10,
    UL_IEPE = 11,
    UL_AI_PARAMS = 12,
    UL_SCAN_CLEAN = 13
}UtMiscFunctions;

#endif // UNITEST_H
