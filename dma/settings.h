#pragma once

namespace SETTINGS {

    //debug
    bool printAll = false;
    float temp = 0;

    // hardware
    bool kmboxnet = false;
    bool kmboxserial = false;
    bool getKeyReady = false;

    //aim
    bool KmboxAim = true;
    bool prediction = true;
    bool show_fov = true;
    float fov = 200;
    int dev_int = 8;
    int aimSteps = 4;
    int key = 0x02;
    int aimBone = 0;
    bool aimAtClosestBone = false;

    //triggerbot
    bool triggerbot = true;
    int TriggerBotDistance = 50;
    int custom_delay = 5;
    int clickDelay = 5;
    int triggerKey = 0x02;

    namespace rifle {
        //aim
        bool KmboxAim = true;
        bool prediction = true;
        bool show_fov = true;
        float fov = 200;
        int dev_int = 8;
        int aimSteps = 4;
        int key = 0x02;
        int aimBone = 0;
        bool aimAtClosestBone = false;

        //triggerbot
        bool triggerbot = true;
        int TriggerBotDistance = 50;
        int custom_delay = 5;
        int clickDelay = 5;
        int triggerKey = 0x02;
    }

    namespace shotgun {
        //aim
        bool KmboxAim = true;
        bool prediction = true;
        bool show_fov = true;
        float fov = 200;
        int dev_int = 8;
        int aimSteps = 4;
        int key = 0x02;
        int aimBone = 0;
        bool aimAtClosestBone = false;

        //triggerbot
        bool triggerbot = true;
        int TriggerBotDistance = 50;
        int custom_delay = 5;
        int clickDelay = 5;
        int triggerKey = 0x02;
    }

    namespace smg {
        //aim
        bool KmboxAim = true;
        bool prediction = true;
        bool show_fov = true;
        float fov = 200;
        int dev_int = 8;
        int aimSteps = 4;
        int key = 0x02;
        int aimBone = 0;
        bool aimAtClosestBone = false;

        //triggerbot
        bool triggerbot = true;
        int TriggerBotDistance = 50;
        int custom_delay = 5;
        int clickDelay = 5;
        int triggerKey = 0x02;
    }

    namespace sniper {
        //aim
        bool KmboxAim = true;
        bool prediction = true;
        bool show_fov = true;
        float fov = 200;
        int dev_int = 8;
        int aimSteps = 4;
        int key = 0x02;
        int aimBone = 0;
        bool aimAtClosestBone = false;

        //triggerbot
        bool triggerbot = true;
        int TriggerBotDistance = 50;
        int custom_delay = 5;
        int clickDelay = 5;
        int triggerKey = 0x02;
    }

    // customize (lots of junk, ignore)

    float box_thickness = 1;
    float box_rounding = 1;
    float skeleton_thickness = 3;
    float snapline_thickness = 2;
    float uselssthickness = 1;
    float thicknessMultiplier = 1;
    float widthMultiplier = 0.6f;
    float rounding = 0.1;
    float thickness_two = 1.0f;
    float width = 0.679;
    float distanceposition = 15;
    float nameposition = -15;
    float corner_box_length = 3.5;
    float corner_box_hight = 3.5;
    float box_length = 1;
    float box_hight = 1;

    //radar
    bool radar = true;
    bool distanceRadar = false;
    float radarZoom = 0.5f;
    float radarSize = 250.f;
    float radarScale = 500.f;
    int radarXPos = 12;
    int radarYPos = 12;

    //esp

    bool lootEsp = false;

    int maxDistance = 300;
    bool teamCheck = true;
    bool selfEsp = false;
    bool normalBox = true;
    bool cornerbox = false;
    bool box3d = false;
    bool skeleton = true;
    bool headBone = true;
    bool distance = true;
    bool nameEsp = false;
    bool lines = false;
    int linesStart = 0;
    bool aimline = false;
    bool ignoreBot = false;
    bool ignoreKnocked = false;

    bool vsync = false;
    bool oldVsync = false;

    bool visCheck = true;

    //menu

    bool particles = false;

    // overlay

    bool fpsCounter = false;
    bool streamProof = false;
    bool oldStreamProof = false;
    bool hdmiFuser = true;
    bool oldHdmiFuser = true;

    // game

    bool inLobby = true;

}

BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf(lpString, ("%d"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
    char lpString[1024];
    sprintf(lpString, ("%f"), nInteger);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}
float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
{
    char szData[32];
    GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);
    return (float)atof(szData);
}

void Save_Settings(LPCSTR path)
{
    //  aim
    //rifle
    WritePrivateProfileInt(("riKmboxAim"), (("riKmboxAim")), SETTINGS::rifle::KmboxAim, path);
    WritePrivateProfileInt(("riPrediction"), (("riPrediction")), SETTINGS::rifle::prediction, path);
    WritePrivateProfileInt(("rishow_fov"), (("rishow_fov")), SETTINGS::rifle::show_fov, path);
    WritePrivateProfileInt(("ridev_int"), (("ridev_int")), SETTINGS::rifle::dev_int, path);
    WritePrivateProfileFloat(("rifov"), (("rifov")), SETTINGS::rifle::fov, path);
    WritePrivateProfileInt(("riaimAtClosestBone"), (("riaimAtClosestBone")), SETTINGS::rifle::aimAtClosestBone, path);
    WritePrivateProfileInt(("riaimSteps"), (("riaimSteps")), SETTINGS::rifle::aimSteps, path);
    WritePrivateProfileInt(("riaimBone"), (("riaimBone")), SETTINGS::rifle::aimBone, path);
    WritePrivateProfileInt(("riaimkey"), (("riaimkey")), SETTINGS::rifle::key, path);

    //shotgun
    WritePrivateProfileInt(("shKmboxAim"), (("shKmboxAim")), SETTINGS::shotgun::KmboxAim, path);
    WritePrivateProfileInt(("shPrediction"), (("shPrediction")), SETTINGS::shotgun::prediction, path);
    WritePrivateProfileInt(("shshow_fov"), (("shshow_fov")), SETTINGS::shotgun::show_fov, path);
    WritePrivateProfileInt(("shdev_int"), (("shdev_int")), SETTINGS::shotgun::dev_int, path);
    WritePrivateProfileFloat(("shfov"), (("shfov")), SETTINGS::shotgun::fov, path);
    WritePrivateProfileInt(("shaimAtClosestBone"), (("shaimAtClosestBone")), SETTINGS::shotgun::aimAtClosestBone, path);
    WritePrivateProfileInt(("shaimSteps"), (("shaimSteps")), SETTINGS::shotgun::aimSteps, path);
    WritePrivateProfileInt(("shaimBone"), (("shaimBone")), SETTINGS::shotgun::aimBone, path);
    WritePrivateProfileInt(("shaimkey"), (("shaimkey")), SETTINGS::shotgun::key, path);

    //smg
    WritePrivateProfileInt(("smKmboxAim"), (("smKmboxAim")), SETTINGS::smg::KmboxAim, path);
    WritePrivateProfileInt(("smPrediction"), (("smPrediction")), SETTINGS::smg::prediction, path);
    WritePrivateProfileInt(("smshow_fov"), (("smshow_fov")), SETTINGS::smg::show_fov, path);
    WritePrivateProfileInt(("smdev_int"), (("smdev_int")), SETTINGS::smg::dev_int, path);
    WritePrivateProfileFloat(("smfov"), (("smfov")), SETTINGS::smg::fov, path);
    WritePrivateProfileInt(("smaimAtClosestBone"), (("smaimAtClosestBone")), SETTINGS::smg::aimAtClosestBone, path);
    WritePrivateProfileInt(("smaimSteps"), (("smaimSteps")), SETTINGS::smg::aimSteps, path);
    WritePrivateProfileInt(("smaimBone"), (("smaimBone")), SETTINGS::smg::aimBone, path);
    WritePrivateProfileInt(("smaimkey"), (("smaimkey")), SETTINGS::smg::key, path);

    //sniper
    WritePrivateProfileInt(("snKmboxAim"), (("snKmboxAim")), SETTINGS::sniper::KmboxAim, path);
    WritePrivateProfileInt(("snPrediction"), (("snPrediction")), SETTINGS::sniper::prediction, path);
    WritePrivateProfileInt(("snshow_fov"), (("snshow_fov")), SETTINGS::sniper::show_fov, path);
    WritePrivateProfileInt(("sndev_int"), (("sndev_int")), SETTINGS::sniper::dev_int, path);
    WritePrivateProfileFloat(("snfov"), (("snfov")), SETTINGS::sniper::fov, path);
    WritePrivateProfileInt(("snaimAtClosestBone"), (("snaimAtClosestBone")), SETTINGS::sniper::aimAtClosestBone, path);
    WritePrivateProfileInt(("snaimSteps"), (("snaimSteps")), SETTINGS::sniper::aimSteps, path);
    WritePrivateProfileInt(("snaimBone"), (("snaimBone")), SETTINGS::sniper::aimBone, path);
    WritePrivateProfileInt(("snaimkey"), (("snaimkey")), SETTINGS::sniper::key, path);

    //  trigger
    //rifle
    WritePrivateProfileInt(("ritriggerbot"), (("ritriggerbot")), SETTINGS::rifle::triggerbot, path);
    WritePrivateProfileInt(("riTriggerBotDistance"), (("riTriggerBotDistance")), SETTINGS::rifle::TriggerBotDistance, path);
    WritePrivateProfileInt(("ricustom_delay"), (("ricustom_delay")), SETTINGS::rifle::custom_delay, path);
    WritePrivateProfileInt(("riclickDelay"), (("riclickDelay")), SETTINGS::rifle::clickDelay, path);
    WritePrivateProfileInt(("ritriggerKey"), (("ritriggerKey")), SETTINGS::rifle::triggerKey, path);

    //shotgun
    WritePrivateProfileInt(("shtriggerbot"), (("shtriggerbot")), SETTINGS::shotgun::triggerbot, path);
    WritePrivateProfileInt(("shTriggerBotDistance"), (("shTriggerBotDistance")), SETTINGS::shotgun::TriggerBotDistance, path);
    WritePrivateProfileInt(("shcustom_delay"), (("shcustom_delay")), SETTINGS::shotgun::custom_delay, path);
    WritePrivateProfileInt(("shclickDelay"), (("shclickDelay")), SETTINGS::shotgun::clickDelay, path);
    WritePrivateProfileInt(("shtriggerKey"), (("shtriggerKey")), SETTINGS::shotgun::triggerKey, path);

    //smg
    WritePrivateProfileInt(("smtriggerbot"), (("smtriggerbot")), SETTINGS::smg::triggerbot, path);
    WritePrivateProfileInt(("smTriggerBotDistance"), (("smTriggerBotDistance")), SETTINGS::smg::TriggerBotDistance, path);
    WritePrivateProfileInt(("smcustom_delay"), (("smcustom_delay")), SETTINGS::smg::custom_delay, path);
    WritePrivateProfileInt(("smclickDelay"), (("smclickDelay")), SETTINGS::smg::clickDelay, path);
    WritePrivateProfileInt(("smtriggerKey"), (("smtriggerKey")), SETTINGS::smg::triggerKey, path);

    //sniper
    WritePrivateProfileInt(("sntriggerbot"), (("sntriggerbot")), SETTINGS::sniper::triggerbot, path);
    WritePrivateProfileInt(("snTriggerBotDistance"), (("snTriggerBotDistance")), SETTINGS::sniper::TriggerBotDistance, path);
    WritePrivateProfileInt(("sncustom_delay"), (("sncustom_delay")), SETTINGS::sniper::custom_delay, path);
    WritePrivateProfileInt(("snclickDelay"), (("snclickDelay")), SETTINGS::sniper::clickDelay, path);
    WritePrivateProfileInt(("sntriggerKey"), (("sntriggerKey")), SETTINGS::sniper::triggerKey, path);

    WritePrivateProfileInt(("radarXPos"), (("radarXPos")), SETTINGS::radarXPos, path);
    WritePrivateProfileInt(("radarYPos"), (("radarYPos")), SETTINGS::radarYPos, path);
    WritePrivateProfileInt(("ignoreBot"), (("ignoreBot")), SETTINGS::ignoreBot, path);
    WritePrivateProfileInt(("ignoreKnocked"), (("ignoreKnocked")), SETTINGS::ignoreKnocked, path);
    WritePrivateProfileInt(("linesStart"), (("linesStart")), SETTINGS::linesStart, path);
    WritePrivateProfileInt(("aimline"), (("aimline")), SETTINGS::aimline, path);
    WritePrivateProfileInt(("2dbox"), (("2dbox")), SETTINGS::normalBox, path);
    WritePrivateProfileInt(("skeleton"), (("skeleton")), SETTINGS::skeleton, path);
    WritePrivateProfileInt(("lines"), (("lines")), SETTINGS::lines, path);
    WritePrivateProfileInt(("distance"), (("distance")), SETTINGS::distance, path);
    WritePrivateProfileInt(("Fps"), (("Fps")), SETTINGS::fpsCounter, path);
    WritePrivateProfileInt(("vsync"), (("vsync")), SETTINGS::vsync, path);
    WritePrivateProfileInt(("headBone"), (("headBone")), SETTINGS::headBone, path);
    WritePrivateProfileInt(("nameEsp"), (("nameEsp")), SETTINGS::nameEsp, path);
    WritePrivateProfileInt(("radar"), (("radar")), SETTINGS::radar, path);
    WritePrivateProfileInt(("lootEsp"), (("lootEsp")), SETTINGS::lootEsp, path);
    WritePrivateProfileInt(("radarScale"), (("radarScale")), SETTINGS::radarScale, path);
    WritePrivateProfileInt(("radarSize"), (("radarSize")), SETTINGS::radarSize, path);
    WritePrivateProfileFloat(("radarZoom"), (("radarZoom")), SETTINGS::radarZoom, path);
    WritePrivateProfileInt(("cNormalBoxVisible"), (("cNormalBoxVisible")), COLORS::cNormalBoxVisible, path);
    WritePrivateProfileInt(("cNormalBoxNotVisible"), (("cNormalBoxNotVisible")), COLORS::cNormalBoxNotVisible, path);
    WritePrivateProfileInt(("cSkeletonVisible"), (("cSkeletonVisible")), COLORS::cSkeletonVisible, path);
    WritePrivateProfileInt(("cSkeletonNotVisible"), (("cSkeletonNotVisible")), COLORS::cSkeletonNotVisible, path);
    WritePrivateProfileInt(("cLinesVisible"), (("cLinesVisible")), COLORS::cLinesVisible, path);
    WritePrivateProfileInt(("cLinesNotVisible"), (("cLinesNotVisible")), COLORS::cLinesNotVisible, path);
    WritePrivateProfileInt(("cAimLines"), (("cAimLines")), COLORS::cAimLines, path);
    WritePrivateProfileInt(("cFov"), (("cFov")), COLORS::cFov, path);
}

void Load_Settings(LPCSTR path)
{
    //  aim
    //rifle
    SETTINGS::rifle::aimAtClosestBone = GetPrivateProfileIntA((("riaimAtClosestBone")), (("riaimAtClosestBone")), SETTINGS::rifle::aimAtClosestBone, path);
    SETTINGS::rifle::KmboxAim = GetPrivateProfileIntA((("riKmboxAim")), (("riKmboxAim")), SETTINGS::rifle::KmboxAim, path);
    SETTINGS::rifle::prediction = GetPrivateProfileIntA((("riPrediction")), (("riPrediction")), SETTINGS::rifle::prediction, path);
    SETTINGS::rifle::aimSteps = GetPrivateProfileIntA((("riaimSteps")), (("riaimSteps")), SETTINGS::rifle::aimSteps, path);
    SETTINGS::rifle::dev_int = GetPrivateProfileIntA((("ridev_int")), (("ridev_int")), SETTINGS::rifle::dev_int, path);
    SETTINGS::rifle::key = GetPrivateProfileIntA((("riaimkey")), (("riaimkey")), SETTINGS::rifle::key, path);
    SETTINGS::rifle::show_fov = GetPrivateProfileIntA((("rishow_fov")), (("rishow_fov")), SETTINGS::rifle::show_fov, path);
    SETTINGS::rifle::fov = GetPrivateProfileFloat((("rifov")), (("rifov")), SETTINGS::rifle::fov, path);

    //shotgun
    SETTINGS::shotgun::aimAtClosestBone = GetPrivateProfileIntA((("shaimAtClosestBone")), (("shaimAtClosestBone")), SETTINGS::shotgun::aimAtClosestBone, path);
    SETTINGS::shotgun::KmboxAim = GetPrivateProfileIntA((("shKmboxAim")), (("shKmboxAim")), SETTINGS::shotgun::KmboxAim, path);
    SETTINGS::shotgun::prediction = GetPrivateProfileIntA((("shPrediction")), (("shPrediction")), SETTINGS::shotgun::prediction, path);
    SETTINGS::shotgun::aimSteps = GetPrivateProfileIntA((("shaimSteps")), (("shaimSteps")), SETTINGS::shotgun::aimSteps, path);
    SETTINGS::shotgun::dev_int = GetPrivateProfileIntA((("shdev_int")), (("shdev_int")), SETTINGS::shotgun::dev_int, path);
    SETTINGS::shotgun::key = GetPrivateProfileIntA((("shaimkey")), (("shaimkey")), SETTINGS::shotgun::key, path);
    SETTINGS::shotgun::show_fov = GetPrivateProfileIntA((("shshow_fov")), (("shshow_fov")), SETTINGS::shotgun::show_fov, path);
    SETTINGS::shotgun::fov = GetPrivateProfileFloat((("shfov")), (("shfov")), SETTINGS::shotgun::fov, path);

    //smg
    SETTINGS::smg::aimAtClosestBone = GetPrivateProfileIntA((("smaimAtClosestBone")), (("smaimAtClosestBone")), SETTINGS::smg::aimAtClosestBone, path);
    SETTINGS::smg::KmboxAim = GetPrivateProfileIntA((("smKmboxAim")), (("smKmboxAim")), SETTINGS::smg::KmboxAim, path);
    SETTINGS::smg::prediction = GetPrivateProfileIntA((("smPrediction")), (("smPrediction")), SETTINGS::smg::prediction, path);
    SETTINGS::smg::aimSteps = GetPrivateProfileIntA((("smaimSteps")), (("smaimSteps")), SETTINGS::smg::aimSteps, path);
    SETTINGS::smg::dev_int = GetPrivateProfileIntA((("smdev_int")), (("smdev_int")), SETTINGS::smg::dev_int, path);
    SETTINGS::smg::key = GetPrivateProfileIntA((("smaimkey")), (("smaimkey")), SETTINGS::smg::key, path);
    SETTINGS::smg::show_fov = GetPrivateProfileIntA((("smshow_fov")), (("smshow_fov")), SETTINGS::smg::show_fov, path);
    SETTINGS::smg::fov = GetPrivateProfileFloat((("smfov")), (("smfov")), SETTINGS::smg::fov, path);

    //sniper
    SETTINGS::sniper::aimAtClosestBone = GetPrivateProfileIntA((("snaimAtClosestBone")), (("snaimAtClosestBone")), SETTINGS::sniper::aimAtClosestBone, path);
    SETTINGS::sniper::KmboxAim = GetPrivateProfileIntA((("snKmboxAim")), (("snKmboxAim")), SETTINGS::sniper::KmboxAim, path);
    SETTINGS::sniper::prediction = GetPrivateProfileIntA((("snPrediction")), (("snPrediction")), SETTINGS::sniper::prediction, path);
    SETTINGS::sniper::aimSteps = GetPrivateProfileIntA((("snaimSteps")), (("snaimSteps")), SETTINGS::sniper::aimSteps, path);
    SETTINGS::sniper::dev_int = GetPrivateProfileIntA((("sndev_int")), (("sndev_int")), SETTINGS::sniper::dev_int, path);
    SETTINGS::sniper::key = GetPrivateProfileIntA((("snaimkey")), (("snaimkey")), SETTINGS::sniper::key, path);
    SETTINGS::sniper::show_fov = GetPrivateProfileIntA((("snshow_fov")), (("snshow_fov")), SETTINGS::sniper::show_fov, path);
    SETTINGS::sniper::fov = GetPrivateProfileFloat((("snfov")), (("snfov")), SETTINGS::sniper::fov, path);

    //  trigger
    //rifle
    SETTINGS::rifle::triggerbot = GetPrivateProfileIntA((("ritriggerbot")), (("ritriggerbot")), SETTINGS::rifle::triggerbot, path);
    SETTINGS::rifle::TriggerBotDistance = GetPrivateProfileIntA((("riTriggerBotDistance")), (("riTriggerBotDistance")), SETTINGS::rifle::TriggerBotDistance, path);
    SETTINGS::rifle::custom_delay = GetPrivateProfileIntA((("ricustom_delay")), (("ricustom_delay")), SETTINGS::rifle::custom_delay, path);
    SETTINGS::rifle::clickDelay = GetPrivateProfileIntA((("riclickDelay")), (("riclickDelay")), SETTINGS::rifle::clickDelay, path);
    SETTINGS::rifle::triggerKey = GetPrivateProfileIntA((("ritriggerKey")), (("ritriggerKey")), SETTINGS::rifle::triggerKey, path);

    //shotgun
    SETTINGS::shotgun::triggerbot = GetPrivateProfileIntA((("shtriggerbot")), (("shtriggerbot")), SETTINGS::shotgun::triggerbot, path);
    SETTINGS::shotgun::TriggerBotDistance = GetPrivateProfileIntA((("shTriggerBotDistance")), (("shTriggerBotDistance")), SETTINGS::shotgun::TriggerBotDistance, path);
    SETTINGS::shotgun::custom_delay = GetPrivateProfileIntA((("shcustom_delay")), (("shcustom_delay")), SETTINGS::shotgun::custom_delay, path);
    SETTINGS::shotgun::clickDelay = GetPrivateProfileIntA((("shclickDelay")), (("shclickDelay")), SETTINGS::shotgun::clickDelay, path);
    SETTINGS::shotgun::triggerKey = GetPrivateProfileIntA((("shtriggerKey")), (("shtriggerKey")), SETTINGS::shotgun::triggerKey, path);

    //smg
    SETTINGS::smg::triggerbot = GetPrivateProfileIntA((("smtriggerbot")), (("smtriggerbot")), SETTINGS::smg::triggerbot, path);
    SETTINGS::smg::TriggerBotDistance = GetPrivateProfileIntA((("smTriggerBotDistance")), (("smTriggerBotDistance")), SETTINGS::smg::TriggerBotDistance, path);
    SETTINGS::smg::custom_delay = GetPrivateProfileIntA((("smcustom_delay")), (("smcustom_delay")), SETTINGS::smg::custom_delay, path);
    SETTINGS::smg::clickDelay = GetPrivateProfileIntA((("smclickDelay")), (("smclickDelay")), SETTINGS::smg::clickDelay, path);
    SETTINGS::smg::triggerKey = GetPrivateProfileIntA((("smtriggerKey")), (("smtriggerKey")), SETTINGS::smg::triggerKey, path);

    //sniper
    SETTINGS::sniper::triggerbot = GetPrivateProfileIntA((("sntriggerbot")), (("sntriggerbot")), SETTINGS::sniper::triggerbot, path);
    SETTINGS::sniper::TriggerBotDistance = GetPrivateProfileIntA((("snTriggerBotDistance")), (("snTriggerBotDistance")), SETTINGS::sniper::TriggerBotDistance, path);
    SETTINGS::sniper::custom_delay = GetPrivateProfileIntA((("sncustom_delay")), (("sncustom_delay")), SETTINGS::sniper::custom_delay, path);
    SETTINGS::sniper::clickDelay = GetPrivateProfileIntA((("snclickDelay")), (("snclickDelay")), SETTINGS::sniper::clickDelay, path);
    SETTINGS::sniper::triggerKey = GetPrivateProfileIntA((("sntriggerKey")), (("sntriggerKey")), SETTINGS::sniper::triggerKey, path);

    SETTINGS::radarXPos = GetPrivateProfileIntA((("radarXPos")), (("radarXPos")), SETTINGS::radarXPos, path);
    SETTINGS::radarYPos = GetPrivateProfileIntA((("radarYPos")), (("radarYPos")), SETTINGS::radarYPos, path);
    SETTINGS::ignoreBot = GetPrivateProfileIntA((("ignoreBot")), (("ignoreBot")), SETTINGS::ignoreBot, path);
    SETTINGS::ignoreKnocked = GetPrivateProfileIntA((("ignoreKnocked")), (("ignoreKnocked")), SETTINGS::ignoreKnocked, path);
    SETTINGS::linesStart = GetPrivateProfileIntA((("linesStart")), (("linesStart")), SETTINGS::linesStart, path);
    SETTINGS::aimline = GetPrivateProfileIntA((("aimline")), (("aimline")), SETTINGS::aimline, path);
    SETTINGS::aimBone = GetPrivateProfileIntA((("aimBone")), (("aimBone")), SETTINGS::aimBone, path);
    SETTINGS::normalBox = GetPrivateProfileIntA((("2dbox")), (("2dbox")), SETTINGS::normalBox, path);
    SETTINGS::skeleton = GetPrivateProfileIntA((("skeleton")), (("skeleton")), SETTINGS::skeleton, path);
    SETTINGS::lines = GetPrivateProfileIntA((("lines")), (("lines")), SETTINGS::lines, path);
    SETTINGS::distance = GetPrivateProfileIntA((("distance")), (("distance")), SETTINGS::distance, path);
    SETTINGS::fpsCounter = GetPrivateProfileIntA((("Fps")), (("Fps")), SETTINGS::fpsCounter, path);
    SETTINGS::vsync = GetPrivateProfileIntA((("vsync")), (("vsync")), SETTINGS::vsync, path);
    SETTINGS::headBone = GetPrivateProfileIntA((("headBone")), (("headBone")), SETTINGS::headBone, path);
    SETTINGS::nameEsp = GetPrivateProfileIntA((("nameEsp")), (("nameEsp")), SETTINGS::nameEsp, path);
    SETTINGS::radarSize = GetPrivateProfileIntA((("radarSize")), (("radarSize")), SETTINGS::radarSize, path);
    SETTINGS::radarZoom = GetPrivateProfileFloat((("radarZoom")), (("radarZoom")), SETTINGS::radarZoom, path);
    SETTINGS::radarScale = GetPrivateProfileIntA((("radarScale")), (("radarScale")), SETTINGS::radarScale, path);
    SETTINGS::radar = GetPrivateProfileIntA((("radar")), (("radar")), SETTINGS::radar, path);
    SETTINGS::lootEsp = GetPrivateProfileIntA((("lootEsp")), (("lootEsp")), SETTINGS::lootEsp, path);
    COLORS::cNormalBoxVisible = GetPrivateProfileIntA((("cNormalBoxVisible")), (("cNormalBoxVisible")), COLORS::cNormalBoxVisible, path);
    COLORS::cNormalBoxNotVisible = GetPrivateProfileIntA((("cNormalBoxNotVisible")), (("cNormalBoxNotVisible")), COLORS::cNormalBoxNotVisible, path);
    COLORS::cSkeletonVisible = GetPrivateProfileIntA((("cSkeletonVisible")), (("cSkeletonVisible")), COLORS::cSkeletonVisible, path);
    COLORS::cSkeletonNotVisible = GetPrivateProfileIntA((("cSkeletonNotVisible")), (("cSkeletonNotVisible")), COLORS::cSkeletonNotVisible, path);
    COLORS::cLinesVisible = GetPrivateProfileIntA((("cLinesVisible")), (("cLinesVisible")), COLORS::cLinesVisible, path);
    COLORS::cLinesNotVisible = GetPrivateProfileIntA((("cLinesNotVisible")), (("cLinesNotVisible")), COLORS::cLinesNotVisible, path);
    COLORS::cAimLines = GetPrivateProfileIntA((("cAimLines")), (("cAimLines")), COLORS::cAimLines, path);
    COLORS::cFov = GetPrivateProfileIntA((("cFov")), (("cFov")), COLORS::cFov, path);
}