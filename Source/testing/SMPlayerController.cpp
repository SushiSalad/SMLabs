// Fill out your copyright notice in the Description page of Project Settings.

#include "SMPlayerController.h"
#include "SMHUD.h"

ASMPlayerController::ASMPlayerController(const FObjectInitializer& ObjectInitializer) : APlayerController(ObjectInitializer) {
	
}

ASMHUD* ASMPlayerController::GetASMHUD() const {
	return Cast<ASMHUD>(GetHUD());
}
