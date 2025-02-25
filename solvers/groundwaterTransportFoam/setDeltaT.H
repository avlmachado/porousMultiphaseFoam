/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Description
    Timestep for groundwaterFoam solver

    \*---------------------------------------------------------------------------*/

if(adjustTimeStep)
{

    //- Timestep using number of Picard's iterations
    if (timeStepControl == "Picard")
    {
        scalar deltaTFact=1;
        if(iterPicard == maxIterPicard)
        {
            deltaTFact = dTFactDecrease;
            iterStability=0;
        }
        else if (iterPicard <= minIterPicard)
        {
            iterStability++;
        }

        if (iterStability == nIterStability)
        {
            deltaTFact = dTFactIncrease;
            iterStability = 0;
        }

        runTime.setDeltaT
            (
                min(
                    deltaTFact*runTime.deltaTValue(),
                    maxDeltaT
                )
            );
    }

    //- Timestep using h variation
    else if(timeStepControl == "dthetamax")
    {
        scalar dtForTheta = dthetamax / (dthetadTmax+SMALL);

        runTime.setDeltaT
            (
                min(
                    dtForTheta,
                    min(
                        1.2*runTime.deltaTValue(),
                        maxDeltaT
                    )
                )
            );
    }

    //- Timestep using C variation
    else if(timeStepControl == "dCmax")
    {
        scalar dCmax = max(dCmax_rel*gMax(C),dCmax_abs);
        scalar dtForC =  dCmax/(dCdTmax+SMALL);
        scalar newDeltaT = min(dtForC, 1.2*runTime.deltaTValue());
        
        runTime.setDeltaT
            (
                min
                (
                    newDeltaT,
                    maxDeltaT
                )
            );

    }   

    if (event.ndates() > 0)
    {
        const scalar timeToNextEvent = event.dates()[currentEvent+1]-runTime.timeOutputValue();
        scalar nSteps =  timeToNextEvent/runTime.deltaTValue();
        if ((nSteps < labelMax) && (nSteps > 0))
        {
            const label nStepsToNextEvent = label(max(nSteps, 1) + 0.99);
            runTime.setDeltaTNoAdjust(timeToNextEvent/nStepsToNextEvent);
        }
    }
}

Info<< "deltaT = " <<  runTime.deltaTValue()
    << " ; dtheta = " << dtheta
    << " ; dtheta avg = " << dtheta_avg
    << " ; dC = " << dC << endl;

// ************************************************************************* //
