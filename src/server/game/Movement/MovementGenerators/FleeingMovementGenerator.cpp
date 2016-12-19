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

#include "Creature.h"
#include "CreatureAI.h"
#include "FleeingMovementGenerator.h"
#include "PathGenerator.h"
#include "ObjectAccessor.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"
#include "VMapFactory.h"

#define MIN_QUIET_DISTANCE 28.0f
#define MAX_QUIET_DISTANCE 43.0f

template<class T>
void FleeingMovementGenerator<T>::DoInitialize(T* unit)
{
    if (!unit)
        return;

    i_nextMoveTime.Reset(50);

    if (unit->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
        return;

    if (unit->IsMovementPreventedByCasting())
    {
        unit->CastStop();
        return;
    }

    unit->AddUnitState(UNIT_FLAG_FLEEING);
    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);    

    if (!unit->IsAlive())
        return;

    if (unit->ToCreature())
    {
        if (Unit* victim = unit->GetVictim())
        {
            unit->InterruptSpell(CURRENT_MELEE_SPELL);
            unit->SendMeleeAttackStop(victim);
        }        
    }
    
    // send to clients the order to immobilize the unit and make it face a random direction.
    Movement::MoveSplineInit init(unit);
    init.MoveTo(unit->GetPosition(), false, false);
    init.SetFacing(frand(0.0f, 2 * static_cast<float>(M_PI)));
    init.Launch();

    unit->StopMoving();
    unit->DisableSpline();    

    unit->ClearUnitState(UNIT_STATE_MOVING);
    unit->AddUnitState(UNIT_STATE_FLEEING_MOVE);
}

template<class T>
void FleeingMovementGenerator<T>::DoReset(T* unit)
{
    DoInitialize(unit);
}

template<class T>
bool FleeingMovementGenerator<T>::DoUpdate(T* unit, uint32 diff)
{
    if (!unit || !unit->IsAlive())
        return false;    

    if (unit->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
    {
        unit->ClearUnitState(UNIT_STATE_FLEEING_MOVE);
        return true;
    }

    if (i_nextMoveTime.Passed())
    {
        // currently moving, update location
        unit->AddUnitState(UNIT_STATE_FLEEING_MOVE);

        if (unit->movespline->Finalized())
            i_nextMoveTime.Reset(urand(800, 1500));
    }
    else
    {
        // waiting for next move
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
        {
            unit->StopMoving();
            unit->DisableSpline();

            // start moving
            unit->AddUnitState(UNIT_STATE_FLEEING_MOVE);

            float dist_from_caster, angle_to_caster;
            if (Unit* fright = ObjectAccessor::GetUnit(*unit, i_frightGUID))
            {
                dist_from_caster = fright->GetDistance(unit);
                if (dist_from_caster > 0.2f)
                    angle_to_caster = fright->GetAngle(unit);
                else
                    angle_to_caster = frand(0, 2 * static_cast<float>(M_PI));
            }
            else
            {
                dist_from_caster = 0.0f;
                angle_to_caster = frand(0, 2 * static_cast<float>(M_PI));
            }

            float dist, angle;
            if (dist_from_caster < MIN_QUIET_DISTANCE)
            {
                dist = frand(0.4f, 1.3f)*(MIN_QUIET_DISTANCE - dist_from_caster);
                angle = angle_to_caster + frand(-static_cast<float>(M_PI) / 8, static_cast<float>(M_PI) / 8);
            }
            else if (dist_from_caster > MAX_QUIET_DISTANCE)
            {
                dist = frand(0.4f, 1.0f)*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE);
                angle = -angle_to_caster + frand(-static_cast<float>(M_PI) / 4, static_cast<float>(M_PI) / 4);
            }
            else    // we are inside quiet range
            {
                dist = frand(0.6f, 1.2f)*(MAX_QUIET_DISTANCE - MIN_QUIET_DISTANCE);
                angle = frand(0, 2 * static_cast<float>(M_PI));
            }

            Position pos = unit->GetFirstCollisionPosition(dist, angle);

            unit->UpdateSpeed(MOVE_WALK);
            unit->UpdateSpeed(MOVE_RUN);

            Movement::MoveSplineInit init(unit);
            init.MoveTo(pos.m_positionX, pos.m_positionY, pos.m_positionZ, true, false);
            init.Launch();

            Movement::Location loc = unit->movespline->ComputePosition();

            if (unit->movespline->onTransport)
            {
                Position& pos = unit->m_movementInfo.transport.pos;
                pos.m_positionX = loc.x;
                pos.m_positionY = loc.y;
                pos.m_positionZ = loc.z;
                pos.SetOrientation(loc.orientation);

                if (TransportBase* transport = unit->GetDirectTransport())
                    transport->CalculatePassengerPosition(loc.x, loc.y, loc.z, &loc.orientation);
            }

            unit->UpdatePosition(loc.x, loc.y, loc.z, loc.orientation);            
        }
    }

    return true;
}

template<>
void FleeingMovementGenerator<Player>::DoFinalize(Player* unit)
{
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    unit->ClearUnitState(UNIT_STATE_FLEEING | UNIT_STATE_FLEEING_MOVE);
    unit->StopMoving();
    unit->DisableSpline();
}

template<>
void FleeingMovementGenerator<Creature>::DoFinalize(Creature* unit)
{
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    unit->ClearUnitState(UNIT_STATE_FLEEING | UNIT_STATE_FLEEING_MOVE);
    if (unit->GetVictim())
        unit->SetTarget(unit->EnsureVictim()->GetGUID());
}

template void FleeingMovementGenerator<Player>::DoInitialize(Player*);
template void FleeingMovementGenerator<Creature>::DoInitialize(Creature*);
template void FleeingMovementGenerator<Player>::DoReset(Player*);
template void FleeingMovementGenerator<Creature>::DoReset(Creature*);
template bool FleeingMovementGenerator<Player>::DoUpdate(Player*, uint32 diff);
template bool FleeingMovementGenerator<Creature>::DoUpdate(Creature*, uint32 diff);

void TimedFleeingMovementGenerator::Finalize(Unit* owner)
{
    owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner->ClearUnitState(UNIT_STATE_FLEEING|UNIT_STATE_FLEEING_MOVE);
    if (Unit* victim = owner->GetVictim())
    {
        if (owner->IsAlive())
        {
            owner->AttackStop();
            owner->ToCreature()->AI()->AttackStart(victim);
        }
    }
}

bool TimedFleeingMovementGenerator::Update(Unit* owner, uint32 time_diff)
{
    if (!owner->IsAlive())
        return false;

    if (owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
    {
        owner->ClearUnitState(UNIT_STATE_FLEEING_MOVE);
        return true;
    }

    i_totalFleeTime.Update(time_diff);
    if (i_totalFleeTime.Passed())
        return false;

    // This calls grant-parent Update method hiden by FleeingMovementGenerator::Update(Creature &, uint32) version
    // This is done instead of casting Unit& to Creature& and call parent method, then we can use Unit directly
    return MovementGeneratorMedium< Creature, FleeingMovementGenerator<Creature> >::Update(owner, time_diff);
}
