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
#include "ConfusedMovementGenerator.h"
#include "PathGenerator.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"
#include "VMapFactory.h"

template<class T>
void ConfusedMovementGenerator<T>::DoInitialize(T* unit)
{
    unit->AddUnitState(UNIT_STATE_CONFUSED);
    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->GetPosition(i_x, i_y, i_z);

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
    init.MoveTo(i_x, i_y, i_z, false, false);
    init.SetFacing(frand(0.0f, 2 * static_cast<float>(M_PI)));
    init.SetWalk(true);
    init.Launch();

    unit->ClearUnitState(UNIT_STATE_MOVING);
    unit->AddUnitState(UNIT_STATE_CONFUSED_MOVE);
}

template<class T>
void ConfusedMovementGenerator<T>::DoReset(T* unit)
{
    i_nextMoveTime.Reset(0);

    if (!unit->IsAlive() || unit->IsStopped())
        return;

    unit->StopMoving();
    unit->DisableSpline();
    unit->AddUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
}

template<class T>
bool ConfusedMovementGenerator<T>::DoUpdate(T* unit, uint32 diff)
{
    if (unit->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return true;

    if (i_nextMoveTime.Passed())
    {
        // currently moving, update location
        unit->AddUnitState(UNIT_STATE_CONFUSED_MOVE);

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
            unit->AddUnitState(UNIT_STATE_CONFUSED_MOVE);

            float dest = 4.0f * (float)rand_norm() - 2.0f;

            Position pos;
            pos.Relocate(i_x, i_y, i_z);
            unit->MovePositionToFirstCollision(pos, dest, 0.0f);

            // additional vmap checking, related with not full algorythm in MovePositionToFirstCollision
            bool col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(unit->GetMapId(), unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ() + 0.5f, pos.m_positionX, pos.m_positionY, pos.m_positionZ + 0.5f, pos.m_positionX, pos.m_positionY, pos.m_positionZ, -0.5f);
            // collision occured
            if (col)
            {
                // move back a bit
                pos.m_positionX -= CONTACT_DISTANCE * std::cos(unit->GetOrientation());
                pos.m_positionY -= CONTACT_DISTANCE * std::sin(unit->GetOrientation());
                if (Map* map = unit->GetMap())
                    pos.m_positionZ = map->GetHeight(unit->GetPhaseMask(), pos.m_positionX, pos.m_positionY, pos.m_positionZ + 2.8f, true);
            }

            PathGenerator path(unit);
            path.SetPathLengthLimit(30.0f);
            bool result = path.CalculatePath(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            if (!result || (path.GetPathType() & PATHFIND_NOPATH))
            {
                i_nextMoveTime.Reset(100);
                return true;
            }

            unit->UpdateSpeed(MOVE_WALK);

            Movement::MoveSplineInit init(unit);
            init.MovebyPath(path.GetPath());
            init.SetWalk(true);
            init.Launch();

            Movement::Location loc = unit->movespline->ComputePosition();

            if (unit->movespline->onTransport)
            {
                Position& tpos = unit->m_movementInfo.transport.pos;
                tpos.m_positionX = loc.x;
                tpos.m_positionY = loc.y;
                tpos.m_positionZ = loc.z;
                tpos.SetOrientation(loc.orientation);

                if (TransportBase* transport = unit->GetDirectTransport())
                    transport->CalculatePassengerPosition(loc.x, loc.y, loc.z, &loc.orientation);
            }

            unit->UpdatePosition(loc.x, loc.y, loc.z, loc.orientation);
        }
    }

    return true;
}

template<>
void ConfusedMovementGenerator<Player>::DoFinalize(Player* unit)
{
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    unit->StopMoving();
    unit->DisableSpline();
}

template<>
void ConfusedMovementGenerator<Creature>::DoFinalize(Creature* unit)
{
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
    unit->ClearUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_CONFUSED_MOVE);
    if (unit->GetVictim())
        unit->SetTarget(unit->EnsureVictim()->GetGUID());
}

template void ConfusedMovementGenerator<Player>::DoInitialize(Player*);
template void ConfusedMovementGenerator<Creature>::DoInitialize(Creature*);
template void ConfusedMovementGenerator<Player>::DoReset(Player*);
template void ConfusedMovementGenerator<Creature>::DoReset(Creature*);
template bool ConfusedMovementGenerator<Player>::DoUpdate(Player*, uint32 diff);
template bool ConfusedMovementGenerator<Creature>::DoUpdate(Creature*, uint32 diff);
