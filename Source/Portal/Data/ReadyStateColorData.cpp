// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ReadyStateColorData.h"

FLinearColor UReadyStateColorData::GetColor(EReadyState State) const
{
    if (const FLinearColor* FoundColor = CharacterStateColorData.Find(State))
    {
        return *FoundColor;
    }
    return FLinearColor::White; // ±âº»°ª

}
