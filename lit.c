       /*********************************************************************
        *********************************************************************
        **                                                                 **
        **                                                                 **
        **                            LITERALS                             **
        **                                                                 **
        **                                                                 **
        **  Revision History:  SCO #23  (12/11/84)                         **
        **                     SCO #31  (02/12/86)       {SCO31.DOC}       **
        **                     SCO #33  (06/03/86)       {SCO33.DOC}       **
        **                     SCO #34  (10/28/86)       {SCO34.DOC}       **
        **                     SCO #36  (07/16/86)       {SCO36.DOC}       **
        **                     SCO #48  (03/21/87)       {SCO48.DOC}       **
        **                                                                 **
        *********************************************************************
        *********************************************************************/





#define POWER_FAIL_SENSE_LATCH 0x0080
#define INTERRUPT_CONTROLLER_PORT_1 0x00C0
#define INTERRUPT_CONTROLLER_PORT_2 0x00C2
#define INSTRUCTION_WORD_1 0x13
#define INSTRUCTION_WORD_2 0x20
#define INSTRUCTION_WORD_3 0x01
#define END_OF_PARITY_INTERRUPT 0x60
#define END_OF_EXCEPTION_INTERRUPT 0x61
#define END_OF_REFRESH_INTERRUPT 0x62
#define END_OF_CYCLE_INTERRUPT 0x63
#define END_OF_SHORT_INTERRUPT 0x67
#define INTERRUPT_CONTROLLER_MASK_0 0x0F0
#define INTERRUPT_CONTROLLER_MASK_1 0x0FC
#define INTERRUPT_CONTROLLER_MASK_2 0x0F8
#define INTERRUPT_CONTROLLER_MASK_3 0x0F4
#define INTERVAL_TIMER_PORT_1 0x0D2
#define INTERVAL_TIMER_PORT_2 0x0D6
#define CONTROL_WORD 0x74
#define DATA_WORD_1 0x12
#define DATA_WORD_2 0x7A
#define CONTROL_WORD_87 0x033C
#define DPM_CONTROL_WORD_87 0x0F3C
#define BIT_CONTROL_WORD_87 0x0F3C
#define INTERFACE_1_START_CODE 0x01
#define INTERFACE_2_START_CODE 0x01
#define HCP_NORMAL 0x00
#define HCP_TEST_MODE 0x01
#define HCP_ADVANCE 0x02
#define HCP_RESET 0x03
#define HCP_ALTITUDE_HOLD_RESET 0x04
#define HCP_FAIL 0x07
      
#define IS =
#define VALID 0x55
#define NO_COMPUTED_DATA 0x66
#define FUNCTIONAL_TEST 0x99
#define TEST_MODE 0x0AA     /*{SCO36.DOC}*/
#define FLIGHT_MODE 0x55      /*{SCO36.DOC}*/
#define POSITIVE_BIAS 0        /*{SCO31.DOC}*/
#define NEGATIVE_BIAS 1        /*{SCO31.DOC}*/
#define RESET 0x0AA
#define PASS 0x55
#define ON 0x55
#define TO_VOR 0x55
#define FROM_VOR 0x99
#define ILS 0x55   /*{SCO48.DOC}*/
#define VOR 0x0AA   /*{SCO48.DOC}*/
#define INVALID 0x0AA
#define OFF 0x0AA
#define PUSHED 0x55
#define SET 0x55
#define FAIL 0x0AA
#define FAILED 0x0FF
#define VMC 4
#define IMC 5
#define CRZ 6
#define TOGA 7
#define BLANK 10
#define NEGATIVE_SIGN 11
            
#define ERROR_DISPLAY 0x5554
#define PFM_KEY_COUNTER 0x5555
#define PFM_TIMER 0x5556
#define COMPUTER_FAULT 0x5557
#define KEY1 5
#define KEY2 4
#define KEY3 3
#define KEY4 2
#define KEY5 1
#define KEY6 0
#define DISPLAY_GENERATOR_PORT 0x0C000
#define DISPLAY_GENERATOR_HALT_CODE 3
#define DISPLAY_GENERATOR_START_CODE 4
#define DISPLAY_HALT_ENABLE_CODE 0
#define DG_HALT 0x0005
#define LOAD_ROTATION_ANGLE 0x0007
#define LOAD_PRESENT_POSITION 0x0008
#define DRAW_VECTOR 0x0009
#define LOAD_BORDERS 0x000D
#define ZERO_DEGREE_ANGLE 0
#define NINETY_DEGREE_ANGLE 0x4000
#define NEG_NINETY_DEGREE_ANGLE 0x0C000
#define ONE_EIGHTY_DEGREE_ANGLE 0x8000
#define FIRST_HALF_OF_BUFFER 0
#define SECOND_HALF_OF_BUFFER 750
#define WINDOW_COMPARATOR_FAIL 0
#define WINDOW_COMPARATOR_PASS 7
#define TWENTY_CYCLES 20
#define FORTY_CYCLES 40
      
#define IOS1_CODE_CHECKSUM_ERROR 0x11     /*{SCO31.DOC}*/
#define IOS1_RAM_ERROR 0x12     /*{SCO31.DOC}*/
#define IOS1_80186_ERROR 0x13     /*{SCO31.DOC}*/
#define IOS1_8087_ERROR 0x14     /*{SCO31.DOC}*/
#define IOS1_CONVERTOR_ERROR 0x15     /*{SCO31.DOC}*/
#define IOS1_HUD_POWER_SUPPLY_ERROR 0x16     /*{SCO31.DOC}*/
#define IOS2_CODE_CHECKSUM_ERROR 0x21     /*{SCO31.DOC}*/
#define IOS2_RAM_ERROR 0x22     /*{SCO31.DOC}*/
#define IOS2_80186_ERROR 0x23     /*{SCO31.DOC}*/
#define IOS2_8087_ERROR 0x24     /*{SCO31.DOC}*/
#define IOS2_CONVERTOR_ERROR 0x25     /*{SCO31.DOC}*/
#define CLP_CODE_CHECKSUM_ERROR 0x31     /*{SCO31.DOC}*/
#define CLP_RAM_ERROR 0x32     /*{SCO31.DOC}*/
#define CLP_8086_ERROR 0x33     /*{SCO31.DOC}*/
#define CLP_8087_ERROR 0x34     /*{SCO31.DOC}*/
#define SM_CODE_CHECKSUM_ERROR 0x41     /*{SCO31.DOC}*/
#define SM_RAM_ERROR 0x42     /*{SCO31.DOC}*/
#define SM_8086_ERROR 0x43     /*{SCO31.DOC}*/
#define SM_8087_ERROR 0x44     /*{SCO31.DOC}*/
#define SYSTEM_MONITOR_INACTIVE 0x45     /*{SCO31.DOC}*/
#define DISPLAY_GENERATOR_ERROR 0x51     /*{SCO31.DOC}*/
#define SYMBOL_MISPOSITIONED_1 0x52     /*{SCO31.DOC}*/
#define OHU_VIDEO_FAIL 0x63     /*{SCO31.DOC}*/
#define OHU_HVPS_FAIL 0x64     /*{SCO31.DOC}*/
#define OHU_95_OR_30_VOLT_SUPPLY_FAIL 0x65     /*{SCO31.DOC}*/
#define SYMBOL_MISPOSITIONED_2 0x71     /*{SCO31.DOC}*/
#define WINDOW_COMPARATOR_ERROR 0x77     /*{SCO31.DOC}*/
#define DEU_VERTICAL_DEFLECTION_FAIL 0x7A     /*{SCO31.DOC}*/
#define DEU_HORIZONTAL_DEFLECTION_FAIL 0x7B     /*{SCO31.DOC}*/
#define DEU_DC_SUPPLY_FAIL 0x7C     /*{SCO31.DOC}*/
#define DEU_BOOST_SUPPLY_FAIL 0x7D     /*{SCO31.DOC}*/
#define DEU_DEFLECTION_SUPPLY_FAIL 0x7E     /*{SCO31.DOC}*/
#define TEST_ERROR_DISPLAY 0x88
#define DISPLAY_GENERATOR_TEST 0x89
#define HCP_FAILURE 0x91      /*{SCO31.DOC}*/
#define RSU_FAILURE 0x0A1     /*{SCO34.DOC}*/
#define COMBINER_NOT_LOCKED 0x0B1     /*{SCO33.DOC}*/
#define EIGHTH_SECOND 3
#define ONE_SIXTH_SECOND 3
#define QUARTER_SECOND 5
#define ONE_THIRD_SECOND 6
#define HALF_SECOND 10
#define ONE_SECOND 20
#define ONE_POINT_FIVE_SECONDS 30
#define TWO_SECONDS 40
#define FIVE_SECONDS 100
                  
#define HALF_PI 1.57079633F
#define PI 3.14159265F
#define TWO_PI 6.28318531F
#define TEN_KNOTS 16.9F
#define TEN_DEGREES 0.1745F
#define FIFTEEN_DEGREES 0.2618F
#define MAXIMUM_NUMBER 99999.9F
                  
#define FOREVER while (1)

