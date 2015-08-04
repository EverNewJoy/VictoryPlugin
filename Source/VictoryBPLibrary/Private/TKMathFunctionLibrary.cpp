/*
	By TK-Master
*/
#include "VictoryBPLibraryPrivatePCH.h"

#include "StaticMeshResources.h"

//~~~ PhysX ~~~
#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
//~~~~~~~~~~~
 
//UTKMathFunctionLibrary
 
float UTKMathFunctionLibrary::GetConsoleVariableFloat(FString VariableName)
{
	static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(*VariableName);

	if (CVar)
	{
		return CVar->GetValueOnGameThread();
		//return CVar->GetValueOnAnyThread();
	}
	 
	return 0.f;
}

int32 UTKMathFunctionLibrary::GetConsoleVariableInt(FString VariableName)
{
	static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(*VariableName);

	if (CVar)
	{
		return CVar->GetValueOnGameThread();
		//return CVar->GetValueOnAnyThread();
	}
	return 0;
}

float UTKMathFunctionLibrary::NegateFloat(float A)
{
	return -A;
}

int32 UTKMathFunctionLibrary::NegateInt(int32 A)
{
	return -A;
}

FVector2D UTKMathFunctionLibrary::NegateVector2D(FVector2D A)
{
	return -A;
}

FVector UTKMathFunctionLibrary::SetVectorLength(FVector A, float size)
{
	return A.GetSafeNormal() * size;
}

FVector UTKMathFunctionLibrary::VectorRadiansToDegrees(FVector RadVector)
{
	return FVector::RadiansToDegrees(RadVector);
}

FVector UTKMathFunctionLibrary::VectorDegreesToRadians(FVector DegVector)
{
	return FVector::DegreesToRadians(DegVector);
}

int32 UTKMathFunctionLibrary::RoundToLowerMultiple(int32 A, int32 Multiple, bool skipSelf)
{
	int32 result = (A / Multiple) * Multiple;
	if (skipSelf && result == A && result != 0)
	{
		return ((A-1) / Multiple) * Multiple;
	}
	return result;
}

int32 UTKMathFunctionLibrary::RoundToUpperMultiple(int32 A, int32 Multiple, bool skipSelf)
{
	if (skipSelf || FMath::Fmod(A, Multiple) != 0)
	{
		A = ((A + Multiple) / Multiple) * Multiple;
	}
	return A;
}

int32 UTKMathFunctionLibrary::RoundToNearestMultiple(int32 A, int32 Multiple)
{
	return ((A + Multiple / 2) / Multiple) * Multiple;
}

bool UTKMathFunctionLibrary::IsPowerOfTwo(int32 x)
{
	//return x && ((x&-x) == x);
	return FMath::IsPowerOfTwo(x);
}

bool UTKMathFunctionLibrary::IsMultipleOf(int32 A, int32 Multiple)
{
	return FMath::Fmod(A, Multiple) == 0;
}

bool UTKMathFunctionLibrary::IsEvenNumber(float A)
{
	return FMath::Fmod(A, 2) == 0;
}

FVector UTKMathFunctionLibrary::ClosestPointOnSphereToLine(FVector SphereOrigin, float SphereRadius, FVector LineOrigin, FVector LineDir)
{
	static FVector OutClosestPoint;
	FMath::SphereDistToLine(SphereOrigin, SphereRadius, LineOrigin, LineDir.GetSafeNormal(), OutClosestPoint);
	return OutClosestPoint;
}

FVector UTKMathFunctionLibrary::ClosestPointOnLineSeqment(FVector Point, FVector LineStart, FVector LineEnd)
{
	return FMath::ClosestPointOnLine(LineStart, LineEnd, Point);
}

bool UTKMathFunctionLibrary::IsPointInsideBox(FVector Point, FVector BoxOrigin, FVector BoxExtent)
{
	FBox Box = FBox::BuildAABB(BoxOrigin, BoxExtent);
	return FMath::PointBoxIntersection(Point, Box);
}

bool UTKMathFunctionLibrary::SphereBoxIntersection(FVector SphereOrigin, float SphereRadius, FVector BoxOrigin, FVector BoxExtent)
{
	FBox Box = FBox::BuildAABB(BoxOrigin, BoxExtent);
	return FMath::SphereAABBIntersection(SphereOrigin, FMath::Square(SphereRadius), Box);
}

bool UTKMathFunctionLibrary::IsLineInsideSphere(FVector LineStart, FVector LineDir, float LineLength, FVector SphereOrigin, float SphereRadius)
{
	return FMath::LineSphereIntersection(LineStart, LineDir, LineLength, SphereOrigin, SphereRadius);
}

bool UTKMathFunctionLibrary::LineExtentBoxIntersection(FBox inBox, FVector Start, FVector End, FVector Extent, FVector& HitLocation, FVector& HitNormal, float& HitTime)
{
	return FMath::LineExtentBoxIntersection(inBox, Start, End, Extent, HitLocation, HitNormal, HitTime);
}

float UTKMathFunctionLibrary::SignedDistancePlanePoint(FVector planeNormal, FVector planePoint, FVector point)
{
	return FVector::DotProduct(planeNormal, (point - planePoint));
}

FVector UTKMathFunctionLibrary::ProjectPointOnLine(FVector LineOrigin, FVector LineDirection, FVector Point)
{
	//FVector linePointToPoint = point - linePoint;
	//float t = FVector::DotProduct(linePointToPoint, lineDir);
	//return linePoint + lineDir * t;

	//FVector closestPoint;
	//OutDistance = FMath::PointDistToLine(point, lineDir, linePoint, closestPoint);
	//return closestPoint;

	FVector SafeDir = LineDirection.GetSafeNormal();
	return LineOrigin + (SafeDir * ((Point - LineOrigin) | SafeDir));
}

void UTKMathFunctionLibrary::ClosestPointsOfLineSegments(FVector Line1Start, FVector Line1End, FVector Line2Start, FVector Line2End, FVector& LinePoint1, FVector& LinePoint2)
{
	FMath::SegmentDistToSegmentSafe(Line1Start, Line1End, Line2Start, Line2End, LinePoint1, LinePoint2);
}

bool UTKMathFunctionLibrary::LineToLineIntersection(FVector& IntersectionPoint, FVector LinePoint1, FVector LineDir1, FVector LinePoint2, FVector LineDir2)
{
	//Are lines coplanar?
	if (!FVector::Coplanar(LinePoint1, LineDir1, LinePoint2, LineDir2, DELTA))
	{
		return false;
	}

	FVector LineDir3 = LinePoint2 - LinePoint1;
	FVector CrossDir1And2 = FVector::CrossProduct(LineDir1, LineDir2);
	FVector CrossDir3And2 = FVector::CrossProduct(LineDir3, LineDir2);

	float s = FVector::DotProduct(CrossDir3And2, CrossDir1And2) / CrossDir1And2.SizeSquared();

	if (s > 1.0f || s < 0.0f)
	{
		return false;
	}
	else
	{
		IntersectionPoint = LinePoint1 + (LineDir1 * s);
		return true;
	}
}

bool UTKMathFunctionLibrary::ClosestPointsOnTwoLines(FVector& closestPointLine1, FVector& closestPointLine2, FVector linePoint1, FVector lineVec1, FVector linePoint2, FVector lineVec2)
{
	float a = FVector::DotProduct(lineVec1, lineVec1);
	float b = FVector::DotProduct(lineVec1, lineVec2);
	float e = FVector::DotProduct(lineVec2, lineVec2);

	float d = a*e - b*b;

	//lines are not parallel
	if (d != 0.0f)
	{
		FVector r = linePoint1 - linePoint2;
		float c = FVector::DotProduct(lineVec1, r);
		float f = FVector::DotProduct(lineVec2, r);

		float s = (b*f - c*e) / d;
		float t = (a*f - c*b) / d;

		closestPointLine1 = linePoint1 + lineVec1 * s;
		closestPointLine2 = linePoint2 + lineVec2 * t;

		return true;
	}
	else
	{
		return false;
	}
}

int32 UTKMathFunctionLibrary::PointOnWhichSideOfLineSegment(FVector linePoint1, FVector linePoint2, FVector point)
{
	FVector lineVec = linePoint2 - linePoint1;
	FVector pointVec = point - linePoint1;

	float dot = FVector::DotProduct(pointVec, lineVec);

	//point is on side of linePoint2, compared to linePoint1
	if (dot > 0)
	{
		//point is on the line segment
		if (pointVec.Size() <= lineVec.Size())
		{
			return 0;
		}

		//point is not on the line segment and it is on the side of linePoint2
		else
		{
			return 2;
		}
	}

	//Point is not on side of linePoint2, compared to linePoint1.
	//Point is not on the line segment and it is on the side of linePoint1.
	else
	{
		return 1;
	}
}

bool UTKMathFunctionLibrary::AreLineSegmentsCrossing(FVector pointA1, FVector pointA2, FVector pointB1, FVector pointB2)
{
	FVector closestPointA;
	FVector closestPointB;
	int32 sideA;
	int32 sideB;

	FVector lineVecA = pointA2 - pointA1;
	FVector lineVecB = pointB2 - pointB1;

	bool valid = ClosestPointsOnTwoLines(closestPointA, closestPointB, pointA1, lineVecA.GetSafeNormal(), pointB1, lineVecB.GetSafeNormal());

	//lines are not parallel
	if (valid)
	{
		sideA = PointOnWhichSideOfLineSegment(pointA1, pointA2, closestPointA);
		sideB = PointOnWhichSideOfLineSegment(pointB1, pointB2, closestPointB);

		if ((sideA == 0) && (sideB == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//lines are parallel
	else
	{
		return false;
	}
}

FVector UTKMathFunctionLibrary::GridSnap(FVector A, float Grid)
{
	return A.GridSnap(Grid);
}

void UTKMathFunctionLibrary::ConvertAnchorToAnchor(UObject* WorldContextObject, FAnchors CurrentAnchor, FMargin Offsets, FAnchors TargetAnchor, FMargin& ConvertedOffsets)
{
	if (CurrentAnchor.Minimum == TargetAnchor.Minimum && CurrentAnchor.Maximum == TargetAnchor.Maximum)
	{
		ConvertedOffsets = Offsets;
		return;
	}

	FVector2D View = FVector2D(1, 1);
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (World && World->IsGameWorld())
	{
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			ViewportClient->GetViewportSize(View);
		}
	}

	FMargin ZeroAnchorOffsets = Offsets;
	//Convert to 0,0 anchor first.
	if (CurrentAnchor.Minimum != FVector2D(0, 0) || CurrentAnchor.Maximum != FVector2D(0, 0))
	{
		ZeroAnchorOffsets.Left = View.X * CurrentAnchor.Minimum.X + Offsets.Left;
		ZeroAnchorOffsets.Top = View.Y * CurrentAnchor.Minimum.Y + Offsets.Top;

		if (CurrentAnchor.Minimum.X != CurrentAnchor.Maximum.X)
		{
			ZeroAnchorOffsets.Right = View.X * CurrentAnchor.Maximum.X - (Offsets.Right + Offsets.Left);
		}
		if (CurrentAnchor.Minimum.Y != CurrentAnchor.Maximum.Y)
		{
			ZeroAnchorOffsets.Bottom = View.Y * CurrentAnchor.Maximum.Y - (Offsets.Bottom + Offsets.Top);
		}

		if (TargetAnchor.Minimum == FVector2D(0, 0) && TargetAnchor.Maximum == FVector2D(0, 0))
		{
			ConvertedOffsets = ZeroAnchorOffsets;
			return;
		}
	}

	//Convert 0,0 anchor offsets to target anchor offsets.
	ConvertedOffsets.Left = (-View.X) * TargetAnchor.Minimum.X + ZeroAnchorOffsets.Left;
	ConvertedOffsets.Top = (-View.Y) * TargetAnchor.Minimum.Y + ZeroAnchorOffsets.Top;

	ConvertedOffsets.Right = TargetAnchor.Minimum.X != TargetAnchor.Maximum.X ? View.X * TargetAnchor.Maximum.X - (ZeroAnchorOffsets.Left + ZeroAnchorOffsets.Right) : ZeroAnchorOffsets.Right;
	ConvertedOffsets.Bottom = TargetAnchor.Minimum.Y != TargetAnchor.Maximum.Y ? View.Y * TargetAnchor.Maximum.Y - (ZeroAnchorOffsets.Top + ZeroAnchorOffsets.Bottom) : ZeroAnchorOffsets.Bottom;
}


float UTKMathFunctionLibrary::ConvertPhysicsLinearVelocity(FVector Velocity, TEnumAsByte<enum ESpeedUnit> SpeedUnit)
{
	if (Velocity.IsZero()) return 0.f;

	float unit = 0;
	switch (SpeedUnit)
	{
		case CentimeterPerSecond:
			unit = 1;
			break;
		case FootPerSecond:
			unit = 0.03280839895013;
			break;
		case MeterPerSecond:
			unit = 0.01;
			break;
		case MeterPerMinute:
			unit = 0.6;
			break;
		case KilometerPerSecond:
			unit = 0.00001;
		case KilometerPerMinute:
			unit = 0.0006;
			break;
		case KilometerPerHour:
			unit = 0.036;
			break;
		case MilePerHour:
			unit = 0.02236936292054;
			break;
		case Knot:
			unit = 0.01943844492441;
			break;
		case Mach:
			unit = 0.00002915451895044;
			break;
		case SpeedOfLight:
			unit = 3.335640951982E-11;
			break;
		case YardPerSecond:
			unit = 0.01093613298338;
			break;
		default:
			break;
	};

	return Velocity.Size() * unit;
}

FVector UTKMathFunctionLibrary::GetVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName, bool DrawDebugInfo)
{
	
	//FTransform Transform = Target->GetComponentTransform();
	//FVector LocalLinearVelocity = Transform.InverseTransformVectorNoScale(Target->GetPhysicsLinearVelocity());
	//FVector LocalAngularVelocity = Transform.InverseTransformVectorNoScale(Target->GetPhysicsAngularVelocity());
	//FVector ResultPointVelocity = LocalLinearVelocity + FVector::CrossProduct(FVector::DegreesToRadians(LocalAngularVelocity), Transform.InverseTransformVectorNoScale(Point - Target->GetCenterOfMass()));
	

	if (!Target) return FVector::ZeroVector;

	//You can actually get it from the physx body instance instead.
	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI && BI->IsValidBodyInstance())
	{
		FVector PointVelocity = BI->GetUnrealWorldVelocityAtPoint(Point);

		UWorld* TheWorld = Target->GetWorld();
		if (DrawDebugInfo && TheWorld)
		{
			DrawDebugPoint(TheWorld, Point, 10, FColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f)), false, -1.0f);
			DrawDebugString(TheWorld, Point, FString::SanitizeFloat(PointVelocity.Size()), NULL, FColor::White, 0.0f);
		}

		return PointVelocity;
	}
	return FVector::ZeroVector;
}

void UTKMathFunctionLibrary::SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName)
{
	if (!Target) return;

	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI && BI->IsValidBodyInstance())
	{
		BI->COMNudge = Offset;
		BI->UpdateMassProperties();
	}
}
