/*
	By TK-Master
*/
#pragma once

//For the Anchor Conversion
#include "Runtime/UMG/Public/UMG.h"

#include "TKMathFunctionLibrary.generated.h"
  
/* Speed Units Enum. */
UENUM()
enum ESpeedUnit
{
	/* Centimeter / second (cm/s). This is default unreal velocity unit. */
	CentimeterPerSecond,

	/* Foot / second (ft/s). */
	FootPerSecond,

	/* Meter / second (m/s). */
	MeterPerSecond,

	/* Meter / minute (m/min). */
	MeterPerMinute,

	/* Kilometer / second (km/s). */
	KilometerPerSecond,

	/* Kilometer / minute (km/min). */
	KilometerPerMinute,

	/*Kilometer / hour (km/h). */
	KilometerPerHour,

	/* Mile / hour (mph). */
	MilePerHour,

	/* Knot (kn). Nautical mile per hour. */
	Knot,

	/* Mach (speed of sound) (M) at standard atm. */
	Mach,

	/* Speed of light. */
	SpeedOfLight,

	/* Yard / second. */
	YardPerSecond
};

 
UCLASS()
class VICTORYBPLIBRARY_API UTKMathFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Console")
	static float GetConsoleVariableFloat(FString VariableName);
	
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Console")
	static int32 GetConsoleVariableInt(FString VariableName);
	 
	//Reverses the sign (- or +) of a float.
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Float")
	static float NegateFloat(float A);

	//Reverses the sign (- or +) of an integer.
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Integer")
	static int32 NegateInt(int32 A);

	//Reverses the sign (- or +) of a Vector2D.
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector2D")
	static FVector2D NegateVector2D(FVector2D A);

	//Changes the size (length) of a Vector to the given size (normalized vector * size).
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector SetVectorLength(FVector A, float size);

	//Converts radians to degrees.
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector VectorRadiansToDegrees(FVector RadVector);

	//Converts degrees to radians.
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector VectorDegreesToRadians(FVector DegVector);

	/**
	* Rounds an integer to the lower multiple of the given number.
	* If Skip Self is set to True it will skip to the previous multiple if the integer rounds to itself.
	* @param Multiple - The multiple number to round to.
	* @param skipSelf - Skip to the previous multiple if the integer rounds to itself.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Integer")
	static int32 RoundToLowerMultiple(int32 A, int32 Multiple = 32, bool skipSelf = false);

	/**
	* Rounds an integer to the upper multiple of the given number.
	* If Skip Self is set to True it will skip to the next multiple if the integer rounds to itself.
	* @param Multiple - The multiple number to round to.
	* @param skipSelf - Skip to the next multiple if the integer rounds to itself.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Integer")
	static int32 RoundToUpperMultiple(int32 A, int32 Multiple = 32, bool skipSelf = false);

	/** Rounds an integer to the nearest multiple of the given number. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Integer")
	static int32 RoundToNearestMultiple(int32 A, int32 Multiple = 32);

	/** Returns true if the integer is a power of two number. */
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "PwrOf2"), Category = "Victory BP Library|TK-Master Math|Integer")
	static bool IsPowerOfTwo(int32 x);

	/** Returns true if the integer is a multiple of the given number. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Integer")
	static bool IsMultipleOf(int32 A, int32 Multiple);

	/** Returns true if the number is even (false if it's odd). */
	UFUNCTION(BlueprintPure, meta = (CompactNodeTitle = "isEven"), Category = "Victory BP Library|TK-Master Math|Float")
	static bool IsEvenNumber(float A);

	/**
	* Find closest point on a Sphere to a Line.
	* When line intersects Sphere, then closest point to LineOrigin is returned.
	* @param SphereOrigin		Origin of Sphere
	* @param SphereRadius		Radius of Sphere
	* @param LineOrigin		Origin of line
	* @param LineDir			Direction of line.
	* @return Closest point on sphere to given line.
	*/ 
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector ClosestPointOnSphereToLine(FVector SphereOrigin, float SphereRadius, FVector LineOrigin, FVector LineDir);

	/** Find the point on line segment from LineStart to LineEnd which is closest to Point. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector ClosestPointOnLineSeqment(FVector Point, FVector LineStart, FVector LineEnd);

	/** Returns true if the given Point vector is within a box (defined by BoxOrigin and BoxExtent). */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool IsPointInsideBox(FVector Point, FVector BoxOrigin, FVector BoxExtent);

	/** Determines whether a line intersects a sphere. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool IsLineInsideSphere(FVector LineStart, FVector LineDir, float LineLength, FVector SphereOrigin, float SphereRadius);

	/**
	* Swept-Box vs Box test.
	* Sweps a box defined by Extend from Start to End points and returns whether it hit a box or not plus the hit location and hit normal if successful.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool LineExtentBoxIntersection(FBox inBox, FVector Start, FVector End, FVector Extent, FVector& HitLocation, FVector& HitNormal, float& HitTime);

	/**
	* Get the shortest distance between a point and a plane.
	* The output is signed so it holds information as to which side of the plane normal the point is.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static float SignedDistancePlanePoint(FVector planeNormal, FVector planePoint, FVector point);

	/**
	* Returns a vector point which is a projection from a point to a line (defined by the vector couple LineOrigin, LineDirection).
	* The line is infinite (in both directions).
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector ProjectPointOnLine(FVector LineOrigin, FVector LineDirection, FVector Point);

	/**
	* Performs a sphere vs box intersection test.
	* @param SphereOrigin the center of the sphere being tested against the box.
	* @param SphereRadius the size of the sphere being tested.
	* @param BoxOrigin the box origin being tested against.
	* @param BoxExtent the box extend (width, depth, height).
	* @return Whether the sphere/box intersect or not.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool SphereBoxIntersection(FVector SphereOrigin, float SphereRadius, FVector BoxOrigin, FVector BoxExtent);

	/**
	* Find closest points between 2 line segments.
	* @param	(Line1Start, Line1End)	defines the first line segment.
	* @param	(Line2Start, Line2End)	defines the second line segment.
	* @param	LinePoint1		Closest point on segment 1 to segment 2.
	* @param	LinePoint2		Closest point on segment 2 to segment 1.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static void ClosestPointsOfLineSegments(FVector Line1Start, FVector Line1End, FVector Line2Start, FVector Line2End, FVector& LinePoint1, FVector& LinePoint2);

	/**
	* Calculate the intersection point of two infinitely long lines. Returns true if lines intersect, otherwise false.
	* Note that in 3d, two lines do not intersect most of the time.
	* So if the two lines are not in the same plane, use Closest Points On Two Lines instead.
	* @param IntersectionPoint The intersection point of the lines. Returns zero if the lines do not intersect or the operation fails.
	* @param LinePoint1 Line point of the first line.
	* @param LineDir1 Line direction (normal) of the first line. Should be a normalized vector.
	* @param LinePoint2 Line point of the second line.
	* @param LineDir2 Line direction (normal) of the second line. Should be a normalized vector.
	* @return true if lines intersect, otherwise false.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool LineToLineIntersection(FVector& IntersectionPoint, FVector LinePoint1, FVector LineDir1, FVector LinePoint2, FVector LineDir2);

	/*
	* Calculate the closest points between two infinitely long lines.
	* If lines are intersecting (not parallel) it will return false! Use Line To Line Intersection instead.
	* @param closestPointLine1 The closest point of line1 to line2. Returns zero if the lines intersect.
	* @param closestPointLine2 The closest point of line2 to line1. Returns zero if the lines intersect.
	* @param linePoint1 Line point of the first line.
	* @param lineVec1 Line direction (normal) of the first line. Should be a normalized vector.
	* @param linePoint2 Line point of the second line.
	* @param lineVec2 Line direction (normal) of the second line. Should be a normalized vector.
	* @return true if lines are parallel, false otherwise.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static bool ClosestPointsOnTwoLines(FVector& closestPointLine1, FVector& closestPointLine2, FVector linePoint1, FVector lineVec1, FVector linePoint2, FVector lineVec2);

	/*
	* Returns 0 if point is on the line segment.
	* Returns 1 if point is not on the line segment and it is on the side of linePoint1.
	* Returns 2 if point is not on the line segment and it is on the side of linePoint2.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static int32 PointOnWhichSideOfLineSegment(FVector linePoint1, FVector linePoint2, FVector point);

	/*
	* Returns true if the two line segments are intersecting and not parallel.
	* If you need the intersection point, use Closest Points On Two Lines.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector|Intersection")
	static bool AreLineSegmentsCrossing(FVector pointA1, FVector pointA2, FVector pointB1, FVector pointB2);

	/*Snaps vector to nearest grid multiple.*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static FVector GridSnap(FVector A, float Grid);

	/*Converts UMG layout offsets to another anchor.*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Anchor", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void ConvertAnchorToAnchor(UObject* WorldContextObject, FAnchors CurrentAnchor, FMargin Offsets, FAnchors TargetAnchor, FMargin& ConvertedOffsets);

	/*Converts Physics Linear Velocity to a more useful speed unit.*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|TK-Master Math|Vector")
	static float ConvertPhysicsLinearVelocity(FVector Velocity, TEnumAsByte<enum ESpeedUnit> SpeedUnit);

	//------------------------------------------------------------
	//Physics functions! (why aren't these exposed by Epic yet?!)

	/**
	* Get the current world velocity of a point on a physics-enabled component.
	* @param Point - Point in world space.
	* @param DrawDebugInfo - Draw debug point & string.
	* @param BoneName - If a SkeletalMeshComponent, name of body to get velocity of. 'None' indicates root body.
	* @return The velocity of the point in world space.
	*/
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static FVector GetVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName = NAME_None, bool DrawDebugInfo = false);

	/*
	* Set the physx center of mass offset.
	* Note: this offsets the physx-calculated center of mass (it is not an offset from the pivot point).
	*/
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static void SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName = NAME_None);

};
