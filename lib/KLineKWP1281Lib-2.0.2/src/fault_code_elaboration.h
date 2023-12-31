#ifndef FAULT_CODE_ELABORATION_H
#define FAULT_CODE_ELABORATION_H

const char PROGMEM fault_elaboration_00[] = "-";
const char PROGMEM fault_elaboration_01[] = "Signal Shorted to Plus";
const char PROGMEM fault_elaboration_02[] = "Signal Shorted to Ground";
const char PROGMEM fault_elaboration_03[] = "No Signal";
const char PROGMEM fault_elaboration_04[] = "Mechanical Malfunction";
const char PROGMEM fault_elaboration_05[] = "Input Open";
const char PROGMEM fault_elaboration_06[] = "Signal too High";
const char PROGMEM fault_elaboration_07[] = "Signal too Low";
const char PROGMEM fault_elaboration_08[] = "Control Limit Surpassed";
const char PROGMEM fault_elaboration_09[] = "Adaptation Limit Surpassed";
const char PROGMEM fault_elaboration_10[] = "Adaptation Limit Not Reached";
const char PROGMEM fault_elaboration_11[] = "Control Limit Not Reached";
const char PROGMEM fault_elaboration_12[] = "Adaptation Limit (Mul) Exceeded";
const char PROGMEM fault_elaboration_13[] = "Adaptation Limit (Mul) Not Reached";
const char PROGMEM fault_elaboration_14[] = "Adaptation Limit (Add) Exceeded";
const char PROGMEM fault_elaboration_15[] = "Adaptation Limit (Add) Not Reached";
const char PROGMEM fault_elaboration_16[] = "Signal Outside Specifications";
const char PROGMEM fault_elaboration_17[] = "Control Difference";
const char PROGMEM fault_elaboration_18[] = "Upper Limit";
const char PROGMEM fault_elaboration_19[] = "Lower Limit";
const char PROGMEM fault_elaboration_20[] = "Malfunction in Basic Setting";
const char PROGMEM fault_elaboration_21[] = "Front Pressure Build-up Time too Long";
const char PROGMEM fault_elaboration_22[] = "Front Pressure Reducing Time too Long";
const char PROGMEM fault_elaboration_23[] = "Rear Pressure Build-up Time too Long";
const char PROGMEM fault_elaboration_24[] = "Rear Pressure Reducing Time too Long";
const char PROGMEM fault_elaboration_25[] = "Unknown Switch Condition";
const char PROGMEM fault_elaboration_26[] = "Output Open";
const char PROGMEM fault_elaboration_27[] = "Implausible Signal";
const char PROGMEM fault_elaboration_28[] = "Short to Plus";
const char PROGMEM fault_elaboration_29[] = "Short to Ground";
const char PROGMEM fault_elaboration_30[] = "Open or Short to Plus";
const char PROGMEM fault_elaboration_31[] = "Open or Short to Ground";
const char PROGMEM fault_elaboration_32[] = "Resistance Too High";
const char PROGMEM fault_elaboration_33[] = "Resistance Too Low";
const char PROGMEM fault_elaboration_34[] = "No Elaboration Available";
const char PROGMEM fault_elaboration_35[] = "-";
const char PROGMEM fault_elaboration_36[] = "Open Circuit";
const char PROGMEM fault_elaboration_37[] = "Faulty";
const char PROGMEM fault_elaboration_38[] = "Output won't Switch or Short to Plus";
const char PROGMEM fault_elaboration_39[] = "Output won't Switch or Short to Ground";
const char PROGMEM fault_elaboration_40[] = "Short to Another Output";
const char PROGMEM fault_elaboration_41[] = "Blocked or No Voltage";
const char PROGMEM fault_elaboration_42[] = "Speed Deviation too High";
const char PROGMEM fault_elaboration_43[] = "Closed";
const char PROGMEM fault_elaboration_44[] = "Short Circuit";
const char PROGMEM fault_elaboration_45[] = "Connector";
const char PROGMEM fault_elaboration_46[] = "Leaking";
const char PROGMEM fault_elaboration_47[] = "No Communications or Incorrectly Connected";
const char PROGMEM fault_elaboration_48[] = "Supply voltage";
const char PROGMEM fault_elaboration_49[] = "No Communications";
const char PROGMEM fault_elaboration_50[] = "Setting (Early) Not Reached";
const char PROGMEM fault_elaboration_51[] = "Setting (Late) Not Reached";
const char PROGMEM fault_elaboration_52[] = "Supply Voltage Too High";
const char PROGMEM fault_elaboration_53[] = "Supply Voltage Too Low";
const char PROGMEM fault_elaboration_54[] = "Incorrectly Equipped";
const char PROGMEM fault_elaboration_55[] = "Adaptation Not Successful";
const char PROGMEM fault_elaboration_56[] = "In Limp-Home Mode";
const char PROGMEM fault_elaboration_57[] = "Electric Circuit Failure";
const char PROGMEM fault_elaboration_58[] = "Can't Lock";
const char PROGMEM fault_elaboration_59[] = "Can't Unlock";
const char PROGMEM fault_elaboration_60[] = "Won't Safe";
const char PROGMEM fault_elaboration_61[] = "Won't De-Safe";
const char PROGMEM fault_elaboration_62[] = "No or Incorrect Adjustment";
const char PROGMEM fault_elaboration_63[] = "Temperature Shut-Down";
const char PROGMEM fault_elaboration_64[] = "Not Currently Testable";
const char PROGMEM fault_elaboration_65[] = "Unauthorized";
const char PROGMEM fault_elaboration_66[] = "Not Matched";
const char PROGMEM fault_elaboration_67[] = "Set-Point Not Reached";
const char PROGMEM fault_elaboration_68[] = "Cylinder 1";
const char PROGMEM fault_elaboration_69[] = "Cylinder 2";
const char PROGMEM fault_elaboration_70[] = "Cylinder 3";
const char PROGMEM fault_elaboration_71[] = "Cylinder 4";
const char PROGMEM fault_elaboration_72[] = "Cylinder 5";
const char PROGMEM fault_elaboration_73[] = "Cylinder 6";
const char PROGMEM fault_elaboration_74[] = "Cylinder 7";
const char PROGMEM fault_elaboration_75[] = "Cylinder 8";
const char PROGMEM fault_elaboration_76[] = "Terminal 30 missing";
const char PROGMEM fault_elaboration_77[] = "Internal Supply Voltage";
const char PROGMEM fault_elaboration_78[] = "Missing Messages";
const char PROGMEM fault_elaboration_79[] = "Please Check Fault Codes";
const char PROGMEM fault_elaboration_80[] = "Single-Wire Operation";
const char PROGMEM fault_elaboration_81[] = "Open";
const char PROGMEM fault_elaboration_82[] = "Activated";

const char* const fault_elaborations[] PROGMEM = {
  fault_elaboration_00, fault_elaboration_01, fault_elaboration_02, fault_elaboration_03, fault_elaboration_04, fault_elaboration_05, fault_elaboration_06, fault_elaboration_07, fault_elaboration_08, fault_elaboration_09, fault_elaboration_10, fault_elaboration_11, fault_elaboration_12, fault_elaboration_13, fault_elaboration_14, fault_elaboration_15, fault_elaboration_16, fault_elaboration_17, fault_elaboration_18, fault_elaboration_19, fault_elaboration_20, fault_elaboration_21, fault_elaboration_22, fault_elaboration_23, fault_elaboration_24, fault_elaboration_25, fault_elaboration_26, fault_elaboration_27, fault_elaboration_28, fault_elaboration_29, fault_elaboration_30, fault_elaboration_31, fault_elaboration_32, fault_elaboration_33, fault_elaboration_34, fault_elaboration_35, fault_elaboration_36, fault_elaboration_37, fault_elaboration_38, fault_elaboration_39, fault_elaboration_40, fault_elaboration_41, fault_elaboration_42, fault_elaboration_43, fault_elaboration_44, fault_elaboration_45, fault_elaboration_46, fault_elaboration_47, fault_elaboration_48, fault_elaboration_49, fault_elaboration_50, fault_elaboration_51, fault_elaboration_52, fault_elaboration_53, fault_elaboration_54, fault_elaboration_55, fault_elaboration_56, fault_elaboration_57, fault_elaboration_58, fault_elaboration_59, fault_elaboration_60, fault_elaboration_61, fault_elaboration_62, fault_elaboration_63, fault_elaboration_64, fault_elaboration_65, fault_elaboration_66, fault_elaboration_67, fault_elaboration_68, fault_elaboration_69, fault_elaboration_70, fault_elaboration_71, fault_elaboration_72, fault_elaboration_73, fault_elaboration_74, fault_elaboration_75, fault_elaboration_76, fault_elaboration_77, fault_elaboration_78, fault_elaboration_79, fault_elaboration_80, fault_elaboration_81, fault_elaboration_82
};

#endif
