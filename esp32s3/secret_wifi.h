const char *machineName0 ="_debug_";
const char *machineName1 ="TUKON M";
const char *machineName2 ="KAGE R";


const char *ssid =      "0831A48C3D76-2G";    // robo beya soft bank
const char *password =  "4kp5hr0tf6gw14"; // ポインタアドレスもconst

//const char *ssid =      "HR02a-2BB4E6";    // robo beya soft bank
//const char *password =  "cztsma0dMd"; // ポインタアドレスもconst

const char *const ssid2 = "ctc-g-8f2dc8";
const char *const password2 = "56b4d9a65c555"; // ポインタアドレスもconst

const char *const ssid_matsu = "W01_24DF6AD40C18";// white wifi
const char *const password_matsu = "q4f5g3m7rfyg1em"; // ポインタアドレスもconst

const char *const ssid_koza = "W04_145F947E1480";    // green wifi
const char *const password_koza = "g4ia9bj2gd0gj0j"; // green wifi

const char *mqtt_server = "192.168.100.50"; // white_wifi

const char *mqtt_server2 = "192.168.0.18";

const char *mqtt_server_matsu = "192.168.100.50"; // white wifi
const char *mqtt_server_koza = "192.168.100.50";  // green wifi
// const char* mqtt_server2 = "192.168.0.17";

// const char *PubTopic    = "/tmr";            // Topic to publish //ToukonM Remotecontrol
const char *PubTopic = "/robo_remo";         // Topic to publish //
const char *PubTopic_soushin = "/robo_remo"; // Topic to publish //

const char *PubTopic_matsu = "/tm_rm"; // Topic to publish
const char *PubTopic_koza = "/kr_rm";  // Topic to publish

const char *SubTopic_matsu = "/tm_rma"; // Topic to subscribe
const char *SubTopic_koza = "/kr_rma";  // Topic to subscribe

const char *MQTT_M = "picoW_remocon";
const char *MQTT_M_TM = "toukonM_remo";
const char *MQTT_M_KR = "kagemushaR_remo";

const char *MQTT_U = "robo";
const char *MQTT_U_TM = "robo";
const char *MQTT_U_KR = "robo";

const char *MQTT_P = "hogehoge";
const char *MQTT_P_TM = "hogehoge";
const char *MQTT_P_KR = "hogehoge";
