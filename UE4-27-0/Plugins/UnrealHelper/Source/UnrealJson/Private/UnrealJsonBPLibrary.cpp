#include "UnrealJsonBPLibrary.h"
#include "UnrealJson.h"
#include "JsonObjectConverter.h"
#include "JsonObjectWrapper.h"
#include "Serialization/JsonTypes.h"

UUnrealJsonBPLibrary::UUnrealJsonBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FString UUnrealJsonBPLibrary::ToJson(
	UPARAM(ref) TFieldPath<FProperty>& Wildcard)
{
	check(0);
	return FString(TEXT("undefined"));
}

FString UUnrealJsonBPLibrary::ToJsonSchema(
	UPARAM(ref) TFieldPath<FProperty>& Wildcard)
{
	check(0);
	return FString(TEXT("undefined"));
}

bool UUnrealJsonBPLibrary::FromJson(
	UPARAM(ref) TFieldPath<FProperty>& Wildcard,
	UPARAM(ref) FString& JsonString)
{
	check(0);
	return false;
}
void UUnrealJsonBPLibrary::JsonObjectWrapper_ToObject(struct FJsonObjectWrapper JsonWrapper,
	UPARAM(ref) TFieldPath<FProperty>& Wildcard
)
{
	check(0);
	return;
}

FString PropertyGetAuthoredName(
	FProperty* Property
)
{
	return Property->GetAuthoredName();
}

// Serialize a UProperty to a JSON String maintaining key names for structures
void UUnrealJsonBPLibrary::PropertyPtrAsJsonString(
	FProperty* Property,
	void* Ptr, FString& JsonString, const bool AsSchema)
{
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&JsonString);

	if(FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property) )
	{
		// TODO: PropertyClass may be a result of Cast being called in the blueprint; we want to keep the cast if it's not just a blank UClass.
		// UE_LOG(LogTemp, Warning, TEXT("Reflected class %s"), *ObjectProperty->PropertyClass->GetClass()->GetName());
		// This is the blueprint function where the variable is set.
		// UE_LOG(LogTemp, Warning, TEXT("Reflected class %s"), *((UObject *) Property)->GetClass()->GetName());
		if(false && !ObjectProperty->HasAnyPropertyFlags(EPropertyFlags::CPF_PersistentInstance)) {
			// Return an object reference as a string
			TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(ObjectProperty, Ptr, 0, 0, nullptr);
			FJsonSerializer::Serialize(JsonValue, FString(), Writer);
		}
		else
		{
			FJsonObject JsonObject = FJsonObject();
			// Struct Serialize does not have a flag to maintain authored key names
			ObjectAsJsonObject(JsonObject, ObjectProperty, Ptr, AsSchema);
			if(AsSchema)
			{
					FJsonObject TypedJsonObject = FJsonObject();
					FString RefId = FString::Format(TEXT("/GRACE/{0}"), {ObjectProperty->PropertyClass->GetName() });
					TypedJsonObject.SetStringField("$id", RefId);
					TypedJsonObject.SetStringField("type", "object");
					TypedJsonObject.SetObjectField("properties", MakeShared<FJsonObject>(JsonObject));
					FJsonSerializer::Serialize(MakeShared<FJsonObject>(TypedJsonObject), Writer);
			}
			else
			{
				FJsonSerializer::Serialize(MakeShared<FJsonObject>(JsonObject), Writer);
			}
		}
	}

	else if(FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		FJsonObject JsonObject = FJsonObject();
		StructAsJsonObject(JsonObject, StructProperty, Ptr, AsSchema);
		// Struct Serialize does not have a flag to maintain authored key names
		if(AsSchema)
		{
				FJsonObject TypedJsonObject = FJsonObject();
				FString RefId = FString::Format(TEXT("/GRACE/{0}"), {StructProperty->Struct->GetClass()->GetName()});
				TypedJsonObject.SetStringField("$id", RefId);
				TypedJsonObject.SetStringField("type", "object");
				TypedJsonObject.SetObjectField("properties", MakeShared<FJsonObject>(JsonObject));
				FJsonSerializer::Serialize(MakeShared<FJsonObject>(TypedJsonObject), Writer);
		}
		else
		{
			FJsonSerializer::Serialize(MakeShared<FJsonObject>(JsonObject), Writer);
		}
	}
	else
	{       
		TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, Ptr, 0, 0, nullptr);
		FJsonSerializer::Serialize(JsonValue, FString(), Writer);
		// WriteCommaIfNeeded does not check for PreviousTokenWritten != EJsonToken::None in WriteValue from JsonWriter.h
		JsonString.RemoveFromStart(FString(","), ESearchCase::IgnoreCase);
	}
}

FString UUnrealJsonBPLibrary::GetJsonTypeString(EJson& JsonType) {
  switch (JsonType)
  {
	case EJson::String: return "string";
	case EJson::Number: return "number";
	case EJson::Boolean: return "boolean";
	case EJson::Array: return "string";
	case EJson::Object: return "object";
	// None and Null 
	default: return "object";
  }
}

// Convert a structure to JSON
void UUnrealJsonBPLibrary::PropertyAsJsonObject(FJsonObject& JsonObject,
	FProperty* Property,
	void* ValuePtr, const bool AsSchema) {
        FString VariableName = PropertyGetAuthoredName(Property);
	if(FArrayProperty *ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper Helper(ArrayProperty, ValuePtr);
		int32 Len = Helper.Num();
		TArray<TSharedPtr<FJsonValue>> ValueJsonArray;
		FJsonObject ValueJsonObject = FJsonObject();
		for(int32 Idx = 0; Idx < Len; ++Idx)
		{
			PropertyAsJsonObject(ValueJsonObject, ArrayProperty->Inner, Helper.GetRawPtr(Idx), AsSchema);
			for(auto It = ValueJsonObject.Values.CreateConstIterator(); It; ++It)
			{
				ValueJsonArray.Add(It.Value());
			}
		}
		JsonObject.SetArrayField(VariableName, ValueJsonArray);
	}
	else if(FObjectProperty *ObjectProperty = CastField<FObjectProperty>(Property))
	{
		FJsonObject ValueJsonObject = FJsonObject();
		// Soft path references as string
		if(false && !ObjectProperty->HasAnyPropertyFlags(EPropertyFlags::CPF_PersistentInstance)) {
			TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(ObjectProperty, ValuePtr, 0, 0, nullptr);
			JsonObject.Values.Add(VariableName, JsonValue);
			return;
		}
		ObjectAsJsonObject(ValueJsonObject, ObjectProperty, ValuePtr, AsSchema);
		if(AsSchema)
		{
				FJsonObject TypedJsonObject = FJsonObject();
				FString RefId = FString::Format(TEXT("/GRACE/{0}//{1}"), {ObjectProperty->PropertyClass->GetFName().ToString(), ObjectProperty->GetNameCPP()});
				TypedJsonObject.SetStringField("$id", RefId);
				TypedJsonObject.SetStringField("type", "object");
				TypedJsonObject.SetObjectField("properties", MakeShared<FJsonObject>(ValueJsonObject));
				JsonObject.SetObjectField(VariableName, MakeShared<FJsonObject>(TypedJsonObject));
		}
		else
		{
			JsonObject.SetObjectField(VariableName, MakeShared<FJsonObject>(ValueJsonObject));
		}
	}
	else if(FStructProperty *SubProperty = CastField<FStructProperty>(Property))
	{
		FJsonObject ValueJsonObject = FJsonObject();
		StructAsJsonObject(ValueJsonObject, SubProperty, ValuePtr, AsSchema);
		if(AsSchema)
		{
				FJsonObject TypedJsonObject = FJsonObject();
				FString RefId = FString::Format(TEXT("/GRACE/{0}"), {SubProperty->Struct->GetName()});
				TypedJsonObject.SetStringField("$id", RefId);
				TypedJsonObject.SetStringField("type", "object");
				TypedJsonObject.SetObjectField("properties", MakeShared<FJsonObject>(ValueJsonObject));
				JsonObject.SetObjectField(VariableName, MakeShared<FJsonObject>(TypedJsonObject));
		}
		else
		{
			JsonObject.SetObjectField(VariableName, MakeShared<FJsonObject>(ValueJsonObject));
		}
	}
	else // if(CastField<FBoolProperty>(Property) || CastField<FFloatProperty>(Property) || CastField<FStrProperty>(Property) || CastField<FIntProperty>(Property) ||  CastField<FEnumProperty>(Property))
      {
		TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, ValuePtr, 0, 0, nullptr);
		if (JsonValue.IsValid())
		{
			if(AsSchema)
			{
				FJsonObject TypedJsonObject = FJsonObject();
				TypedJsonObject.SetStringField("type", GetJsonTypeString(JsonValue->Type));
				TypedJsonObject.SetField("example", JsonValue);
				FString RefId = FString::Format(TEXT("/Unreal/{0}"), {Property->GetClass()->GetName()});
				TypedJsonObject.SetStringField("$id", RefId);
				// Also FNumericProperty->IsEnum()
				if(FByteProperty *EnumProperty = CastField<FByteProperty>(Property))
				{
					UEnum* Enum = EnumProperty->GetIntPropertyEnum();
					TArray<TSharedPtr<FJsonValue>> ValueJsonArray;
					for(int32 EnumIndex = 0; EnumIndex < Enum->NumEnums() - 1; ++EnumIndex) {
						ValueJsonArray.Add(MakeShared<FJsonValueString>(Enum->GetAuthoredNameStringByIndex(EnumIndex)));
					}
					TypedJsonObject.SetArrayField("enum", ValueJsonArray);
					// FindObject is an interesting walk
					TypedJsonObject.SetField("example", MakeShared<FJsonValueString>(Enum->GetAuthoredNameStringByValue(EnumProperty->GetSignedIntPropertyValue(ValuePtr))));
					TypedJsonObject.SetStringField("$id", FString::Format(TEXT("/GRACE/{0}"), {Enum->GetName()}));
				}
				if(FEnumProperty *EnumProperty = CastField<FEnumProperty>(Property))
				{
					UEnum* Enum = EnumProperty->GetEnum();
					// this may just be for enums with more than 255 elements.
				}
				JsonObject.SetObjectField(VariableName, MakeShared<FJsonObject>(TypedJsonObject));
			}
			else
			{
				JsonObject.Values.Add(VariableName, JsonValue);
			}
		}
	}
/*
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unserialized JSON reflected class %s"), *Property->GetClass()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Unserialized JSON reflected name %s"), *Property->GetFName().GetPlainNameString());
	}
*/

}

void UUnrealJsonBPLibrary::ObjectAsJsonObject(FJsonObject& JsonObject,
	FObjectProperty* ObjectProperty,
	void* ObjectPtr, const bool AsSchema) {
	UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectPtr);
	UClass* PropertyClass = Object->GetClass();
	for (TFieldIterator<FProperty> It(PropertyClass); It; ++It)
	{
		FProperty* Property = *It;
		FString VariableName = PropertyGetAuthoredName(Property);
		for(int32 Idx = 0; Idx < Property->ArrayDim; Idx++)
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>((void*) Object, Idx);
			PropertyAsJsonObject(JsonObject, Property, ValuePtr, AsSchema);
		}
	}
}

void UUnrealJsonBPLibrary::StructAsJsonObject(FJsonObject& JsonObject,
	FStructProperty* StructProperty,
	void* StructPtr, const bool AsSchema) {
	for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
	{
		FProperty* Property = *It;
		FString VariableName = PropertyGetAuthoredName(Property);
		for(int32 Idx = 0; Idx < Property->ArrayDim; Idx++)
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructPtr, Idx);
			PropertyAsJsonObject(JsonObject, Property, ValuePtr, AsSchema);
		}
	}
}

// Convert JSON string into a structure
void UUnrealJsonBPLibrary::JsonAsProperty(TSharedPtr<FJsonObject> JsonObject,
	FProperty* Property,
	void* ValuePtr) {
	FString VariableName = PropertyGetAuthoredName(Property);
	if(FArrayProperty *ArrayProperty = CastField<FArrayProperty>(Property))
	{
		TArray<TSharedPtr<FJsonValue>> ValueJsonArray = JsonObject->Values[VariableName].Get()->AsArray();
		FScriptArrayHelper ValueArray(ArrayProperty, ValuePtr);
		ValueArray.Resize(ValueJsonArray.Num());
		FJsonObject ValueJsonObject = FJsonObject();
		for(int32 Idx = 0; Idx < ValueJsonArray.Num(); ++Idx)
		{				
			ValueJsonObject.Values.Add(VariableName, ValueJsonArray[Idx]);
			void* SubValuePtr = ValueArray.GetRawPtr(Idx);
			JsonAsProperty(MakeShared<FJsonObject>(ValueJsonObject), ArrayProperty->Inner, SubValuePtr);
		}
	}
	else if(FObjectProperty *ObjectProperty = CastField<FObjectProperty>(Property))
	{
		// Allow soft reference strings
		if(JsonObject->Values[VariableName].Get()->Type == EJson::String)
		{
			// The internal code does run:
			// if(!ObjectProperty->HasAnyPropertyFlags(EPropertyFlags::CPF_PersistentInstance))
			// ObjectProperty->ImportText(*JsonObject->Values[VariableName].Get()->AsString(), ValuePtr, 0, NULL);
			FJsonObjectConverter::JsonValueToUProperty(JsonObject->Values[VariableName], Property, ValuePtr, 0, 0);		
		}
		else
		{
			TSharedPtr<FJsonObject> SubObject = JsonObject->Values[VariableName].Get()->AsObject();
			JsonAsObject(SubObject, ObjectProperty, ValuePtr);
		}
	}
	else if(FStructProperty *SubProperty = CastField<FStructProperty>(Property))
	{
		TSharedPtr<FJsonObject> SubObject = JsonObject->Values[VariableName].Get()->AsObject();
		JsonAsStruct(SubObject, SubProperty, ValuePtr);
	}
	else
	{
		FJsonObjectConverter::JsonValueToUProperty(JsonObject->Values[VariableName], Property, ValuePtr, 0, 0);		
	}
}

void UUnrealJsonBPLibrary::JsonAsObject(TSharedPtr<FJsonObject> JsonObject,
	FObjectProperty* ObjectProperty,
	void* ObjectPtr) {

	UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectPtr);
	// This could be an access violation
	UClass* PropertyClass = Object->GetClass();

	for (TFieldIterator<FProperty> It(PropertyClass); It; ++It)
	{
		FProperty* Property = *It;
		if(!JsonObject->HasField(PropertyGetAuthoredName(Property))) continue;
		for(int32 Idx = 0; Idx < Property->ArrayDim; Idx++)
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>((void*) Object, Idx);
			JsonAsProperty(JsonObject, Property, ValuePtr);
		}
	}
}

void UUnrealJsonBPLibrary::JsonAsStruct(TSharedPtr<FJsonObject> JsonObject,
	FStructProperty* StructProperty,
	void* StructPtr) {
	for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
	{
		FProperty* Property = *It;
		if(!JsonObject->HasField(PropertyGetAuthoredName(Property))) continue;
		for(int32 Idx = 0; Idx < Property->ArrayDim; Idx++)
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructPtr, Idx);
			JsonAsProperty(JsonObject, Property, ValuePtr);
		}
	}
}

void UUnrealJsonBPLibrary::FromJson_Object(TSharedPtr<FJsonObject> JsonObject,
	FProperty* WildcardProperty,
	void* PropertyPtr) {
	FObjectProperty* ObjectProperty = CastField<FObjectProperty>(WildcardProperty);
	FStructProperty* StructProperty = CastField<FStructProperty>(WildcardProperty);
	if(ObjectProperty)
	{
		JsonAsObject(JsonObject, ObjectProperty, PropertyPtr);
	}
	else if(StructProperty)
	{
		JsonAsStruct(JsonObject, StructProperty, PropertyPtr);
	}
	else
	{
		JsonAsProperty(JsonObject, WildcardProperty, PropertyPtr);
	}
}

bool UUnrealJsonBPLibrary::FromJson_Generic(
	FProperty* WildcardProperty,
	void* PropertyPtr, FString& JsonString)
{
	FObjectProperty* ObjectProperty = CastField<FObjectProperty>(WildcardProperty);
	FStructProperty* StructProperty = CastField<FStructProperty>(WildcardProperty);
	FString JsonObjectString = StructProperty || ObjectProperty ? JsonString : FString::Printf(TEXT("{\"%s\": %s}"), *PropertyGetAuthoredName(WildcardProperty), *JsonString);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonObjectString);
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
/*
		if(StructProperty && StructProperty->Struct == FJsonObjectWrapper::StaticStruct())
		{
			UScriptStruct* Struct = StructProperty->Struct;
			UE_LOG(LogTemp, Warning, TEXT("Reflection on %s not implemented"), *Struct->GetStructCPPName());
			return false;
		}
*/
		FromJson_Object(JsonObject, WildcardProperty, PropertyPtr);
		return true;
	}
	return false;
}

FString UUnrealJsonBPLibrary::JsonObjectWrapper_ToString(struct FJsonObjectWrapper JsonWrapper, const bool PrettyPrint)
{
	if(!JsonWrapper.JsonObject.IsValid())
	{
		return FString();
	}
	FString JsonString;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&JsonString);
	FJsonSerializer::Serialize(JsonWrapper.JsonObject.ToSharedRef(), Writer);
	return JsonString;
}

bool UUnrealJsonBPLibrary::JsonObject_WithFieldPath(FJsonObject& JsonObject, const FString& WithFieldPath) {
	TArray<FString> PathParts;
	int32 PathLength = WithFieldPath.ParseIntoArray(PathParts, *FString("."), true);
	if(PathLength == 0)
	{
		return true;
	}
	if(!JsonObject.HasField(PathParts[0]))
	{
		return false;
	}
	const TSharedPtr<FJsonObject> JsonPtr = MakeShared<FJsonObject>(JsonObject);
	const TSharedPtr<FJsonObject>* CurrentPointer = &JsonPtr;
	for(int32 Idx = 0; Idx < PathLength; Idx++)
	{
		if(!(*CurrentPointer)->TryGetObjectField(PathParts[Idx], CurrentPointer))
		{
			// Allow the ending field to be any field type
			if((Idx == PathLength - 1) && (*CurrentPointer)->HasField(PathParts[Idx]))
			{
				break;
			}
			else
			{
				return false;
			}
		}
	}
	JsonObject = *(*CurrentPointer).Get();
	return true;
}

void UUnrealJsonBPLibrary::JsonWrapper_FromString(UPARAM(ref) FString& JsonString, const FString WithFieldPath, EJsonFieldExistsEnum& FieldExists, FJsonObjectWrapper& JsonWrapper) {
	TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
	JsonWrapper.JsonObject = JsonObject;
	if(JsonString.Len() > 0)
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
			if(WithFieldPath.Len() > 0)
			{
				FJsonObject Root = *JsonObject.Get();   
				if(JsonObject_WithFieldPath(Root, WithFieldPath))
				{
					(&JsonWrapper)->JsonObject = MakeShared<FJsonObject>(Root);
					FieldExists = EJsonFieldExistsEnum::Changed;
				}
				else
				{
					FieldExists = EJsonFieldExistsEnum::NotChanged;
					return;
				}
			}
			else
			{
				(&JsonWrapper)->JsonObject = JsonObject;
				FieldExists = EJsonFieldExistsEnum::Changed;
				return;
			}
		}
		else
		{
			FieldExists = EJsonFieldExistsEnum::NotChanged;
			return;
		}
	}
	else
	{
		FieldExists = EJsonFieldExistsEnum::NotChanged;
		return;
	}
}

// undocumented; from source: DeterminesOutputType = "Parameter", DynamicOutputParam="Param"
// UFUNCTION(BlueprintCallable, Category="Utilities",  meta=(DeterminesOutputType="Interface", DynamicOutputParam="OutActors"))
// static void GetAllActorsWithInterface(TSubclassOf<UInterface> Interface, TArray<AActor*>& OutActors);
void UUnrealJsonBPLibrary::JsonWrapper_FromField(struct FJsonObjectWrapper InJsonWrapper, FString WithFieldPath, EJsonFieldExistsEnum& FieldExists, FJsonObjectWrapper& OutJsonWrapper) {
	// JsonObjectWrapper is poorly defined in UE 4.22
	OutJsonWrapper.JsonObject = InJsonWrapper.JsonObject.IsValid() ? InJsonWrapper.JsonObject : (TSharedPtr<FJsonObject>) (new FJsonObject());
	FJsonObject Root = *OutJsonWrapper.JsonObject.Get();
	if(JsonObject_WithFieldPath(Root, WithFieldPath))
	{
		OutJsonWrapper.JsonObject = MakeShared<FJsonObject>(Root);
		FieldExists = EJsonFieldExistsEnum::Changed;
		return;
	}
	FieldExists = EJsonFieldExistsEnum::NotChanged;
	return;
}