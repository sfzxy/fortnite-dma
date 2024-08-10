#pragma once
#include <cstdint>

namespace offsets {
    uintptr_t temp1 = 0x110;
    uintptr_t temp2 = 0x120;

    uintptr_t Uworld = 0x1178F4B8;
    uintptr_t FNamePool = 0x117284C0;
    uintptr_t FNameKey = 0x18;
    uintptr_t ItemDefinition = 0x368;
    uintptr_t GameInstance = 0x1D8;
    uintptr_t GameState = 0x160;
    uintptr_t PersistentLevel = 0x30;
    uintptr_t LocalPlayers = 0x38;
    uintptr_t PlayerController = 0x30;
    uintptr_t LocalPawn = 0x338;
    uintptr_t Cameramanager = 0x348;
    uintptr_t PlayerState = 0x2B0;
    uintptr_t PawnPrivate = 0x308;
    uintptr_t PlayerArray = 0x2A8;
    uintptr_t Mesh = 0x318;
    uintptr_t ULevelArray = 0x178;
    uintptr_t ULevelCount = 0x180;
    uintptr_t AActorArray = 0xA0;
    uintptr_t AActorCount = 0xA8;
    uintptr_t ComponetToWorld = 0x1c0;
    uintptr_t LastSubmitTime = 0x2E8;
    uintptr_t LastRenderTimeOnScreen = 0x2F0;
    uintptr_t BoneArray = 0x5B8;
    uintptr_t Velocity = 0x168;
    uintptr_t IsDBNO = 0x982; // ( BitField Index -> 4 )
    uintptr_t isDying = 0x758; // ( BitField Index -> 4 )
    uintptr_t TeamId = 0x1211;
    uintptr_t RootComponent = 0x198;
    uintptr_t LocationUnderReticle = 0x26B8;
    uintptr_t bIsABot = 0x29A; // ( BitField Index -> 3 )
    int PlayerWeapon = 0xA68;
    int WeaponData = 0x510;
    int TextPtr = 0x40;
    int NameOffset = 0x90;
}