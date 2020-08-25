// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef TESTING_SMCharacter_generated_h
#error "SMCharacter.generated.h already included, missing '#pragma once' in SMCharacter.h"
#endif
#define TESTING_SMCharacter_generated_h

#define testing_Source_testing_SMCharacter_h_12_SPARSE_DATA
#define testing_Source_testing_SMCharacter_h_12_RPC_WRAPPERS
#define testing_Source_testing_SMCharacter_h_12_RPC_WRAPPERS_NO_PURE_DECLS
#define testing_Source_testing_SMCharacter_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASMCharacter(); \
	friend struct Z_Construct_UClass_ASMCharacter_Statics; \
public: \
	DECLARE_CLASS(ASMCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/testing"), NO_API) \
	DECLARE_SERIALIZER(ASMCharacter)


#define testing_Source_testing_SMCharacter_h_12_INCLASS \
private: \
	static void StaticRegisterNativesASMCharacter(); \
	friend struct Z_Construct_UClass_ASMCharacter_Statics; \
public: \
	DECLARE_CLASS(ASMCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/testing"), NO_API) \
	DECLARE_SERIALIZER(ASMCharacter)


#define testing_Source_testing_SMCharacter_h_12_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ASMCharacter(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ASMCharacter) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASMCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASMCharacter); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASMCharacter(ASMCharacter&&); \
	NO_API ASMCharacter(const ASMCharacter&); \
public:


#define testing_Source_testing_SMCharacter_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASMCharacter(ASMCharacter&&); \
	NO_API ASMCharacter(const ASMCharacter&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASMCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASMCharacter); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASMCharacter)


#define testing_Source_testing_SMCharacter_h_12_PRIVATE_PROPERTY_OFFSET
#define testing_Source_testing_SMCharacter_h_9_PROLOG
#define testing_Source_testing_SMCharacter_h_12_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	testing_Source_testing_SMCharacter_h_12_PRIVATE_PROPERTY_OFFSET \
	testing_Source_testing_SMCharacter_h_12_SPARSE_DATA \
	testing_Source_testing_SMCharacter_h_12_RPC_WRAPPERS \
	testing_Source_testing_SMCharacter_h_12_INCLASS \
	testing_Source_testing_SMCharacter_h_12_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define testing_Source_testing_SMCharacter_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	testing_Source_testing_SMCharacter_h_12_PRIVATE_PROPERTY_OFFSET \
	testing_Source_testing_SMCharacter_h_12_SPARSE_DATA \
	testing_Source_testing_SMCharacter_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	testing_Source_testing_SMCharacter_h_12_INCLASS_NO_PURE_DECLS \
	testing_Source_testing_SMCharacter_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> TESTING_API UClass* StaticClass<class ASMCharacter>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID testing_Source_testing_SMCharacter_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
