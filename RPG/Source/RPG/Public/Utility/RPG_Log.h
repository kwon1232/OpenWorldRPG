#pragma once

#include "CoreMinimal.h"


RPG_API DECLARE_LOG_CATEGORY_EXTERN(RPGLOG, Log, All); //Ãß°¡

#define LOG_CALLINFO (FString(FUNCTION) + TEXT("(") + FString::FromInt(LINE) + TEXT(")"))

#define LOG_S(Verbosity) UE_LOG(RPGLOG, Verbosity, TEXT("%s"), LOG_CALLINFO)

#define LOG(Verbosity, Format, ...) UE_LOG(RPGLOG, Verbosity, TEXT("%s %s"),LOG_CALLINFO, FString::Printf(Format, ##VA_ARGS))



#define RPG_LOG(Verbosity, Format, ...) UE_LOG(RPGLOG, Verbosity, TEXT("%s %s"),LOG_CALLINFO, FString::Printf(Format, ##VA_ARGS))

#define RPG_LOG_WARNING(Format, ...) UE_LOG(RPGLOG, Warning, TEXT("%s %s"),LOG_CALLINFO, FString::Printf(Format, ##VA_ARGS))
#define RPG_LOG_ERROR(Format, ...) UE_LOG(RPGLOG, Error, TEXT("%s %s"),LOG_CALLINFO, *FString::Printf(Format, ##VA_ARGS))

#define RPG_LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##VA_ARGS))
#define RPG_LOG_SCREEN_T(Time, Format, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(Format), ##VA_ARGS))

#define RPG_LOG_SCREEN_ERROR(Format, ...) \
{ \
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##VA_ARGS)); \
RPG_LOG_ERROR(Format, ##VA_ARGS); \
}


#define RPG_CHECK(Expr, ...) {if(!(Expr)) {LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return VA_ARGS;}}
#define RPG_IF_CHECK(Expr, ErrorMsg, ...) {if(!(Expr)) {RPG_LOG_ERROR(ErrorMsg); return VA_ARGS;}}