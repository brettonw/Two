//------------------------------------------------------------------------------
//	Copyright (C) 1997-2000 Bretton Wade
//	All Rights Reserved
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	includes
//------------------------------------------------------------------------------
#include	"precompile.h"
#include	"command.h"

//------------------------------------------------------------------------------
//	class methods
//------------------------------------------------------------------------------
/* void */
Command::Command
(
    const Text&     name
)
{
    myName = name;
}

//------------------------------------------------------------------------------
/* void */
Command::~Command
(
    void
)
{
}

//------------------------------------------------------------------------------
Text
Command::GetName
(
    void
) const
{
    return myName;
}

//------------------------------------------------------------------------------