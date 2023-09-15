// Copyright AppLovin Corporation. All Rights Reserved.

#include "AppLovinMAXUtils.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"


FString AppLovinMAXUtils::SerializeMap(const TMap<FString, FString> &Map)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    for (const TPair<FString, FString> &Entry : Map)
    {
        JsonObject->SetStringField(Entry.Key, Entry.Value);
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return OutputString;
}

FString AppLovinMAXUtils::ParseColor(const FColor &Color)
{
    FString Hex = Color.ToHex(); // RRGGBBAA
    return TEXT("#") + Hex.Right(2) + Hex.Left(6);
}
