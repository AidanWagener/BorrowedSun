#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BSDemoHUD.generated.h"

UCLASS()
class BORROWEDSUN_API ABSDemoHUD : public AHUD
{
    GENERATED_BODY()
public:
    ABSDemoHUD();
    virtual void DrawHUD() override;
private:
    UPROPERTY() TObjectPtr<class UFont> DisplayFont;
    float Scale = 1;
    void Text(const FString& S, float X, float Y, float Size, FLinearColor Color);
    void Panel(float X, float Y, float W, float H, FLinearColor Color);
};
