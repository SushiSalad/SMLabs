// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "testing/SMCharacter.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSMCharacter() {}
// Cross Module References
	TESTING_API UClass* Z_Construct_UClass_ASMCharacter_NoRegister();
	TESTING_API UClass* Z_Construct_UClass_ASMCharacter();
	ENGINE_API UClass* Z_Construct_UClass_ACharacter();
	UPackage* Z_Construct_UPackage__Script_testing();
	ENGINE_API UClass* Z_Construct_UClass_UCharacterMovementComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCapsuleComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
// End Cross Module References
	void ASMCharacter::StaticRegisterNativesASMCharacter()
	{
	}
	UClass* Z_Construct_UClass_ASMCharacter_NoRegister()
	{
		return ASMCharacter::StaticClass();
	}
	struct Z_Construct_UClass_ASMCharacter_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AirSpeedIncreaseLimit_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_AirSpeedIncreaseLimit;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GroundAcceleration_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_GroundAcceleration;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AirAcceleration_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_AirAcceleration;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SMCharacterMovementComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SMCharacterMovementComponent;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SMCapsuleComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SMCapsuleComponent;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FPSCameraComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_FPSCameraComponent;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASMCharacter_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ACharacter,
		(UObject* (*)())Z_Construct_UPackage__Script_testing,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "SMCharacter.h" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirSpeedIncreaseLimit_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirSpeedIncreaseLimit = { "AirSpeedIncreaseLimit", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, AirSpeedIncreaseLimit), METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirSpeedIncreaseLimit_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirSpeedIncreaseLimit_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_GroundAcceleration_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_GroundAcceleration = { "GroundAcceleration", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, GroundAcceleration), METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_GroundAcceleration_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_GroundAcceleration_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirAcceleration_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirAcceleration = { "AirAcceleration", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, AirAcceleration), METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirAcceleration_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirAcceleration_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCharacterMovementComponent_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCharacterMovementComponent = { "SMCharacterMovementComponent", nullptr, (EPropertyFlags)0x00100000000a0009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, SMCharacterMovementComponent), Z_Construct_UClass_UCharacterMovementComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCharacterMovementComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCharacterMovementComponent_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCapsuleComponent_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCapsuleComponent = { "SMCapsuleComponent", nullptr, (EPropertyFlags)0x00100000000a0009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, SMCapsuleComponent), Z_Construct_UClass_UCapsuleComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCapsuleComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCapsuleComponent_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASMCharacter_Statics::NewProp_FPSCameraComponent_MetaData[] = {
		{ "Category", "SMCharacter" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "SMCharacter.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASMCharacter_Statics::NewProp_FPSCameraComponent = { "FPSCameraComponent", nullptr, (EPropertyFlags)0x00100000000a0009, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(ASMCharacter, FPSCameraComponent), Z_Construct_UClass_UCameraComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::NewProp_FPSCameraComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::NewProp_FPSCameraComponent_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASMCharacter_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirSpeedIncreaseLimit,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_GroundAcceleration,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_AirAcceleration,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCharacterMovementComponent,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_SMCapsuleComponent,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASMCharacter_Statics::NewProp_FPSCameraComponent,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASMCharacter_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASMCharacter>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASMCharacter_Statics::ClassParams = {
		&ASMCharacter::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_ASMCharacter_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_ASMCharacter_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASMCharacter_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASMCharacter()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASMCharacter_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASMCharacter, 3062936082);
	template<> TESTING_API UClass* StaticClass<ASMCharacter>()
	{
		return ASMCharacter::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASMCharacter(Z_Construct_UClass_ASMCharacter, &ASMCharacter::StaticClass, TEXT("/Script/testing"), TEXT("ASMCharacter"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASMCharacter);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
