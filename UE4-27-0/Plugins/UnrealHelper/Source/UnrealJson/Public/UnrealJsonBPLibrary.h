#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "JsonObjectConverter.h"
#include "JsonObjectWrapper.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Json/Public/Policies/CondensedJsonPrintPolicy.h"
#include "UnrealJsonBPLibrary.generated.h"

UENUM(BlueprintType)
enum class EJsonFieldExistsEnum : uint8
{
	Changed            UMETA(DisplayName = "Changed"),
	NotChanged         UMETA(DisplayName = "Not Changed"),
};

/* 
 * JSON struct serialization/deserialization
 * from mhoelzl/PropertyTest tutorial on gitlab, and reflection tutorial on forums.
 */
UCLASS()
class UUnrealJsonBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	 * JSON struct serialization/deserialization
	 * in the style of KismetArrayLibrary.h
	 */

	/**
	 *	Convert structure to JSON
	 *	work-around the build in converter because names are mangled:
	 *	- Built-in: FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, StructPtr, JsonString, 0, 0, 0, nullptr, false);
         *	- Property->GetAuthoredName() should be called, not GetName()
         *	- There's a normalization which lowercases the first character and treats ID as a special string
	 *	Convert JSON string into a structure
	 *	- JsonObjectToUStruct did not seem to populate correctly
	 *	- FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructProperty->Struct, StructPtr, 0, 0))
         *	- NOTE: Make Structure must be followed by Set Members or Set to create a value usable with pass-by-reference
	 */

	/*
	// Could be a helper at some point, may be unnecessary.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Class to JSON", Keywords = "Json"), Category = "Unreal Helper BP Library")
      static FString ClassToJson(UPARAM(ref) UObject* Object)
	{
		UE_LOG(LogTemp, Warning, TEXT("Explore class %s"), *Object->GetClass()->GetName());
		FString JsonString = "";
		// PropertyPtrAsJsonString(WildcardProperty, PropertyAddr, JsonString);
		return JsonString;
      }
	*/

	/**
	 *	Serialize from a wildcard property to a JSON string
	 *
	 *	@param    Wildcard        The input to serialize to JSON
	 *	@return   String containing serializable properties as JSON
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Object to JSON", Keywords = "Json", CustomStructureParam = "Wildcard"), Category = "Unreal Helper BP Library", CustomThunk)
        static FString ToJson(UPARAM(ref) TFieldPath<FProperty>& Wildcard);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Object to JSON Schema", Keywords = "Json", CustomStructureParam = "Wildcard"), Category = "Unreal Helper BP Library", CustomThunk)
        static FString ToJsonSchema(UPARAM(ref) TFieldPath<FProperty>& Wildcard);
        static void ObjectAsJsonObject(FJsonObject& JsonObject, FObjectProperty* ObjectProperty, void* ObjectPtr, const bool AsSchema);
        static void StructAsJsonObject(FJsonObject& JsonObject, FStructProperty* StructProperty, void* StructPtr, const bool AsSchema);
        static void PropertyAsJsonObject(FJsonObject& JsonObject, FProperty* Property, void* ValuePtr, const bool AsSchema);
		static FString GetJsonTypeString(EJson& JsonType);

	// FIXME: Property and PropertyPtr should be const
	static void PropertyPtrAsJsonString(
		FProperty* Property,
		void* PropertyPtr, FString& JsonString, const bool AsSchema);
	DECLARE_FUNCTION(execToJson)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		FProperty* WildcardProperty = CastField<FProperty>(Stack.MostRecentProperty);
		void* PropertyAddr = Stack.MostRecentPropertyAddress;
		// P_GET_PROPERTY(FBoolProperty, AsSchema);
		P_FINISH;

		P_NATIVE_BEGIN;
		FString JsonString;
		bool AsSchema = false;
		PropertyPtrAsJsonString(WildcardProperty, PropertyAddr, JsonString, AsSchema);
		*(FString*)RESULT_PARAM = JsonString;
		P_NATIVE_END;
	}

	// Notes: Do not use, still under development
	DECLARE_FUNCTION(execToJsonSchema)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		FProperty* WildcardProperty = CastField<FProperty>(Stack.MostRecentProperty);
		void* PropertyAddr = Stack.MostRecentPropertyAddress;
		P_FINISH;

		P_NATIVE_BEGIN;
		FString JsonString;
		bool AsSchema = true;
		PropertyPtrAsJsonString(WildcardProperty, PropertyAddr, JsonString, AsSchema);
		*(FString*)RESULT_PARAM = JsonString;
		P_NATIVE_END;
	}

	/**
	 *	Deserialize a JSON string into an existing a wildcard property
	 *
	 *	@param    Wildcard        The structure or object to populate
	 *	@param    JsonString       The JSON string to deserialize into the target structure or object
	 *	@return   bool True if json was parseable
	 */

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JSON to Object", Keywords = "Json", CustomStructureParam = "Wildcard", AutoCreateRefTerm="Wildcard"), Category = "Unreal Helper BP Library", CustomThunk)
        static bool FromJson(
		UPARAM(ref) TFieldPath<FProperty>& Wildcard,
		UPARAM(ref) FString& JsonString);
        static void FromJson_Object(TSharedPtr<FJsonObject> JsonObject, FProperty* StructProperty, void* StructPtr);
        static void JsonAsObject(TSharedPtr<FJsonObject> JsonObject, FObjectProperty* ObjectProperty, void* ObjectPtr);
        static void JsonAsStruct(TSharedPtr<FJsonObject> JsonObject, FStructProperty* StructProperty, void* StructPtr);
        static void JsonAsProperty(TSharedPtr<FJsonObject> JsonObject, FProperty* Property, void* ValuePtr);

	static bool FromJson_Generic(
		FProperty* WildcardProperty,
		void* PropertyPtr, FString& JsonString);
	DECLARE_FUNCTION(execFromJson)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		FProperty* WildcardProperty = CastField<FProperty>(Stack.MostRecentProperty);
		void* PropertyAddr = Stack.MostRecentPropertyAddress;
		P_GET_PROPERTY_REF(FStrProperty, JsonString);

		P_FINISH;

		P_NATIVE_BEGIN;
		// MARK_PROPERTY_DIRTY(Stack.Object, WildcardProperty);
		*(bool*)RESULT_PARAM = FromJson_Generic(WildcardProperty, PropertyAddr, JsonString);
		P_NATIVE_END;
	}

	/**
	 *	Work with JsonWrapper to check if a field exists
	 *
	 *	@param    JsonWrapper      (optional) Unreal Json Wrapper over a JsonObject
	 *	@param    JsonString       (optional) JSON string to deserialize into a Json Wrapper
	 *	@param    FieldName        A top level field name to The JSON string to look for in the json object
	 *	@param    OutJsonWrapper The JsonWrapper to populate when the field was found with the resulting object
	 *	@param    FieldExists    Enum of Changed or Not Changed based on field name filter and validity
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JsonObjectWrapper From Field", Keywords = "Json", ExpandEnumAsExecs = "FieldExists", AutoCreateRefTerm="JsonString"), Category = "Unreal Helper BP Library")
	static void JsonWrapper_FromField(struct FJsonObjectWrapper InJsonWrapper, FString WithFieldPath, EJsonFieldExistsEnum& FieldExists, FJsonObjectWrapper& OutJsonWrapper);

	/**
	 *	Work with JsonWrapper to check if a field exists
	 *
	 *	@param    JsonString      JSON string to deserialize into a Json Wrapper
	 *	@param    WithFieldPath   A top level field name or dot separated field path to traverse in the JSON object
	 *	@param    JsonWrapper     Blueprint wrapper over a JsonObject
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "JsonObjectWrapper From String", Keywords = "Json", ExpandEnumAsExecs = "FieldExists", AutoCreateRefTerm="JsonString"), Category = "Unreal Helper BP Library")
	static void JsonWrapper_FromString(UPARAM(ref) FString& JsonString, const FString WithFieldPath, EJsonFieldExistsEnum& FieldExists, FJsonObjectWrapper& JsonWrapper);

	/**
	 *	Work with JsonWrapper to check if a field exists
	 *
	 *	@param    JsonWrapper     Unreal Json Wrapper over a JsonObject
	 *	@return   FString    Serialized JSON as a string
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "To String", Keywords = "Json"), Category = "Unreal Helper BP Library")
	static FString JsonObjectWrapper_ToString(struct FJsonObjectWrapper JsonWrapper, const bool PrettyPrint=false);
	static bool JsonObject_WithFieldPath(FJsonObject& JsonObject, const FString& WithFieldPath);
	
	/**
	 *	Unwrap a JSON wrapper into an existing a wildcard property
	 *
	 *	@param    Wildcard        The structure or object to populate
	 *	@param    JsonWrapper       The Json Wrapper to unwrap into the target structure or object
	 *	@return   void
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "To Object", Keywords = "Json", CustomStructureParam = "Wildcard", AutoCreateRefTerm="Wildcard"), Category = "Unreal Helper BP Library", CustomThunk)
	static void JsonObjectWrapper_ToObject(struct FJsonObjectWrapper JsonWrapper,
		UPARAM(ref) TFieldPath<FProperty>& Wildcard
	);
	DECLARE_FUNCTION(execJsonObjectWrapper_ToObject)
	{
		P_GET_STRUCT(FJsonObjectWrapper, JsonWrapper);
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		FProperty* WildcardProperty = CastField<FProperty>(Stack.MostRecentProperty);
		void* PropertyAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;

		P_NATIVE_BEGIN;
		// MARK_PROPERTY_DIRTY(Stack.Object, WildcardProperty);
		FromJson_Object(JsonWrapper.JsonObject, WildcardProperty, PropertyAddr);
		P_NATIVE_END;
	}

};
