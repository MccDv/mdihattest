#ifndef UNITEST_H
#define UNITEST_H


#ifndef HAT_03

#define HAT_ID_MCC_134 0x0143

#define TC_DISABLED  (0xFF)
#define HAT_ID_MCC_134       0x0143

#define TC_TYPE_J   0
#define TC_TYPE_K   1
#define TC_TYPE_T   2
#define TC_TYPE_E   3
#define TC_TYPE_R   4
#define TC_TYPE_S   5
#define TC_TYPE_B   6
#define TC_TYPE_N   7

#endif  // HAT_03

#ifndef HAT_04

#define HAT_ID_MCC_152       0x0144

    /// Configure channel direction
#define DIO_DIRECTION        0
    /// Configure pull-up/down resistor
#define DIO_PULL_CONFIG      1
    /// Enable pull-up/down resistor
#define DIO_PULL_ENABLE      2
    /// Configure input inversion
#define DIO_INPUT_INVERT     3
    /// Configure input latching
#define DIO_INPUT_LATCH      4
    /// Configure output type
#define DIO_OUTPUT_TYPE      5
    /// Configure interrupt mask
#define DIO_INT_MASK         6

#endif  // HAT_04

#ifndef HAT_05

#define HAT_ID_MCC_172 0x0145

enum SourceType
{
    /// Use a local-only source.
    SOURCE_LOCAL    = 0,
    /// Use a local source and set it as master.
    SOURCE_MASTER   = 1,
    /// Use a separate master source.
    SOURCE_SLAVE    = 2
};

/// A hardware overrun occurred.
#define STATUS_HW_OVERRUN       (0x0001)
/// A scan buffer overrun occurred.
#define STATUS_BUFFER_OVERRUN   (0x0002)
/// The trigger event occurred.
#define STATUS_TRIGGERED        (0x0004)
/// The scan is running (actively acquiring data.)
#define STATUS_RUNNING          (0x0008)

#endif  // HAT_05

#ifndef HAT_06

#define HAT_ID_MCC_128       0x0146

enum AnalogInputMode {
    A_IN_MODE_SE = 0,
    A_IN_MODE_DIFF = 1
};

enum AnalogInputRange {
    A_IN_RANGE_BIP_10V = 0,
    A_IN_RANGE_BIP_5V = 1,
    A_IN_RANGE_BIP_2V = 2,
    A_IN_RANGE_BIP_1V = 3
};

#define A_IN_MODE_SE_FLAG     0x00
#define A_IN_MODE_DIFF_FLAG   0x08
#define A_IN_MODE_BIT_MASK    0x08
#define A_IN_MODE_BIT_POS        3

#endif  // HAT_06

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
    UL_SCAN_CLEAN = 13,
    UL_TRIG_CFG = 14,
    UL_AI_SENSE = 15
}UtMiscFunctions;

#endif // UNITEST_H
