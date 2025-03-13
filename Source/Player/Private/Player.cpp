#include "Player.h"
#include "NotHaloPlayerLogging.h"

#define LOCTEXT_NAMESPACE "FPlayerModule"

void FPlayerModule::StartupModule()
{
    
}

void FPlayerModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FPlayerModule, Player)

DEFINE_LOG_CATEGORY(NotHaloPlayer);