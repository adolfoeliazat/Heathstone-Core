/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_FLEEINGMOVEMENTGENERATOR_H
#define TRINITY_FLEEINGMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "Timer.h"

template<class T>
class FleeingMovementGenerator : public MovementGeneratorMedium< T, FleeingMovementGenerator<T> >
{
public:
    explicit FleeingMovementGenerator(ObjectGuid fright) : i_frightGUID(fright), i_nextMoveTime(0) { }

    void DoInitialize(T*);
    void DoFinalize(T*);
    void DoReset(T*);
    bool DoUpdate(T*, uint32);

    MovementGeneratorType GetMovementGeneratorType() const override { return FLEEING_MOTION_TYPE; }
private:
    ObjectGuid i_frightGUID;
    TimeTracker i_nextMoveTime;
};

class TimedFleeingMovementGenerator : public FleeingMovementGenerator<Creature>
{
    public:
        TimedFleeingMovementGenerator(ObjectGuid fright, uint32 time) :
            FleeingMovementGenerator<Creature>(fright),
            i_totalFleeTime(time) { }

        MovementGeneratorType GetMovementGeneratorType() const override { return TIMED_FLEEING_MOTION_TYPE; }
        bool Update(Unit*, uint32) override;
        void Finalize(Unit*) override;

    private:
        TimeTracker i_totalFleeTime;
};

#endif
