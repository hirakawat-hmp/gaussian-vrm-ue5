// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#include "GVRMSkinningData.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/PlatformFileManager.h"

#if WITH_EDITOR

bool UGVRMBindingData::ImportFromCSV(const FString& CSVFilePath, FString& OutErrorMessage)
{
	// Check if file exists
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*CSVFilePath))
	{
		OutErrorMessage = FString::Printf(TEXT("File not found: %s"), *CSVFilePath);
		return false;
	}

	// Read file contents
	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *CSVFilePath))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *CSVFilePath);
		return false;
	}

	// Parse CSV
	TArray<FString> Lines;
	FileContents.ParseIntoArrayLines(Lines);

	if (Lines.Num() < 2) // At least header + 1 data line
	{
		OutErrorMessage = TEXT("CSV file is empty or has no data rows");
		return false;
	}

	// Clear existing bindings
	Bindings.Empty();
	Bindings.Reserve(Lines.Num() - 1);

	// Parse data lines (skip header at index 0)
	for (int32 i = 1; i < Lines.Num(); ++i)
	{
		const FString& Line = Lines[i];
		if (Line.IsEmpty())
			continue;

		// Parse CSV line: SplatIndex,VertexIndex,BoneIndex,RelativePosX,RelativePosY,RelativePosZ
		TArray<FString> Columns;
		Line.ParseIntoArray(Columns, TEXT(","), true);

		if (Columns.Num() < 6)
		{
			OutErrorMessage = FString::Printf(TEXT("Invalid CSV format at line %d (expected 6 columns, got %d)"),
				i + 1, Columns.Num());
			return false;
		}

		// Parse values
		int32 SplatIndex = FCString::Atoi(*Columns[0]);
		int32 VertexIndex = FCString::Atoi(*Columns[1]);
		int32 BoneIndex = FCString::Atoi(*Columns[2]);
		float RelPosX = FCString::Atof(*Columns[3]);
		float RelPosY = FCString::Atof(*Columns[4]);
		float RelPosZ = FCString::Atof(*Columns[5]);

		// Create binding
		FSplatBindingInfo Binding;
		Binding.SplatIndex = SplatIndex;
		Binding.VertexIndex = VertexIndex;
		Binding.BoneIndex = BoneIndex;
		Binding.RelativePosition = FVector(RelPosX, RelPosY, RelPosZ);

		Bindings.Add(Binding);
	}

	OutErrorMessage = FString::Printf(TEXT("Successfully imported %d splat bindings"), Bindings.Num());
	return true;
}

bool UGVRMBindingData::ImportMetadataFromJSON(const FString& JSONFilePath, FString& OutErrorMessage)
{
	// Check if file exists
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*JSONFilePath))
	{
		OutErrorMessage = FString::Printf(TEXT("File not found: %s"), *JSONFilePath);
		return false;
	}

	// Read file contents
	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *JSONFilePath))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *JSONFilePath);
		return false;
	}

	// Parse JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		OutErrorMessage = TEXT("Failed to parse JSON");
		return false;
	}

	// Read modelScale
	if (JsonObject->HasField(TEXT("modelScale")))
	{
		ModelScale = JsonObject->GetNumberField(TEXT("modelScale"));
	}

	// Read version
	if (JsonObject->HasField(TEXT("version")))
	{
		Version = JsonObject->GetStringField(TEXT("version"));
	}

	// Read boneOperations
	if (JsonObject->HasField(TEXT("boneOperations")))
	{
		BoneOperations.Empty();
		const TArray<TSharedPtr<FJsonValue>>& BoneOpsArray = JsonObject->GetArrayField(TEXT("boneOperations"));

		for (const TSharedPtr<FJsonValue>& BoneOpValue : BoneOpsArray)
		{
			const TSharedPtr<FJsonObject>& BoneOpObj = BoneOpValue->AsObject();
			if (!BoneOpObj.IsValid())
				continue;

			FGVRMBoneOperation BoneOp;

			// Read boneName
			if (BoneOpObj->HasField(TEXT("boneName")))
			{
				BoneOp.BoneName = BoneOpObj->GetStringField(TEXT("boneName"));
			}

			// Read position
			if (BoneOpObj->HasField(TEXT("position")))
			{
				const TSharedPtr<FJsonObject>& PosObj = BoneOpObj->GetObjectField(TEXT("position"));
				BoneOp.PositionOffset = FVector(
					PosObj->GetNumberField(TEXT("x")),
					PosObj->GetNumberField(TEXT("y")),
					PosObj->GetNumberField(TEXT("z"))
				);
			}

			// Read rotation (degrees)
			if (BoneOpObj->HasField(TEXT("rotation")))
			{
				const TSharedPtr<FJsonObject>& RotObj = BoneOpObj->GetObjectField(TEXT("rotation"));
				BoneOp.Rotation = FRotator(
					RotObj->GetNumberField(TEXT("y")), // Pitch
					RotObj->GetNumberField(TEXT("z")), // Yaw
					RotObj->GetNumberField(TEXT("x"))  // Roll
				);
			}

			BoneOperations.Add(BoneOp);
		}
	}

	OutErrorMessage = FString::Printf(TEXT("Successfully imported metadata (scale=%.2f, %d bone operations)"),
		ModelScale, BoneOperations.Num());
	return true;
}

#endif // WITH_EDITOR
