#include "Weapons.h"
#include "NotHaloWeaponsLogging.h"

#define LOCTEXT_NAMESPACE "FWeaponsModule"

void FWeaponsModule::StartupModule()
{
    
}

void FWeaponsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FWeaponsModule, Weapons)

DEFINE_LOG_CATEGORY(NotHaloWeapons);