#include "Core/BSStableId.h"

namespace
{
bool IsCanonicalPart(const FString& Part)
{
    if (Part.IsEmpty() || Part.Len() > 128)
    {
        return false;
    }
    for (const TCHAR Character : Part)
    {
        const bool bAllowed = (Character >= TEXT('a') && Character <= TEXT('z'))
            || (Character >= TEXT('0') && Character <= TEXT('9'))
            || Character == TEXT('.') || Character == TEXT('_') || Character == TEXT('-');
        if (!bAllowed)
        {
            return false;
        }
    }
    return true;
}
}

bool FBSStableId::IsValid() const
{
    return IsCanonicalPart(Namespace) && IsCanonicalPart(Key);
}

FString FBSStableId::ToString() const
{
    return IsValid() ? Namespace + TEXT(":") + Key : FString();
}

bool FBSStableId::operator==(const FBSStableId& Other) const
{
    return Namespace.Equals(Other.Namespace, ESearchCase::CaseSensitive)
        && Key.Equals(Other.Key, ESearchCase::CaseSensitive);
}
