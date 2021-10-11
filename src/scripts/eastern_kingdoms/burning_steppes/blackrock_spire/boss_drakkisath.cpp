/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "scriptPCH.h"

struct boss_drakkisathAI : public ScriptedAI
{
    boss_drakkisathAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    static constexpr uint32 SPELL_FLAMESTRIKE = 16419;
    static constexpr uint32 SPELL_CLEAVE = 15284;
    static constexpr uint32 SPELL_CONFLIGURATION = 16805;
    static constexpr uint32 SPELL_THUNDERCLAP = 15548;
    static constexpr uint32 SPELL_RAGE = 16789;
    static constexpr uint32 SPELL_PIERCEARMOR = 12097;

    uint32 m_uiFlameStrikeTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiConfligurationTimer;
    uint32 m_uiThunderclapTimer;
    uint32 m_uiRageTimer;
    uint32 m_uiPierceArmorTimer;
    uint32 m_uiLeashCheckTimer;
    bool m_bSummoned;
    bool m_bPulledByPet;

    void Reset() override
    {
        m_uiFlameStrikeTimer = 16000;
        m_uiCleaveTimer = 12000;
        m_uiConfligurationTimer = 8000;
        m_uiThunderclapTimer = 1000;
        m_uiRageTimer = 1000;
        m_uiPierceArmorTimer = 5000;
        m_uiLeashCheckTimer = 5000;
        m_bSummoned = false;
        m_bPulledByPet = false;
    }

    void EnterCombat(Unit* pUnit) override
    {
        // Prevent exploit where pet can run through the wall and pull the boss.
        if (Unit* pOwner = pUnit->GetOwner())
            if (!pOwner->IsWithinLOSInMap(m_creature))
                m_bPulledByPet = true;
    }

    void LeashIfOutOfCombatArea(uint32 uiDiff)
    {
        if (m_uiLeashCheckTimer < uiDiff)
            m_uiLeashCheckTimer = 3500;
        else
        {
            m_uiLeashCheckTimer -= uiDiff;
            return;
        }

        if (m_bPulledByPet || (m_creature->GetPositionZ() > 115.0f || m_creature->GetPositionZ() < 105.0f))
        {
            EnterEvadeMode();
            return;
        }
    }

    void UpdateAI(uint32 const uiDiff) override
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        // Prevent exploit
        LeashIfOutOfCombatArea(uiDiff);

        // FlameStrike
        if (m_uiFlameStrikeTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_FLAMESTRIKE) == CAST_OK)
                m_uiFlameStrikeTimer = urand(13000, 14000);
        }
        else
            m_uiFlameStrikeTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(8000, 10000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Confliguration
        if (m_uiConfligurationTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_CONFLIGURATION, 0, m_creature->GetVictim()->GetGUID()) == CAST_OK)
                m_uiConfligurationTimer = 18000;
        }
        else
            m_uiConfligurationTimer -= uiDiff;

        // Thunderclap
        if (m_uiThunderclapTimer < uiDiff)
        {
            if ((m_creature->GetVictim())->GetDistance(m_creature) < 10.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP) == CAST_OK)
                    m_uiThunderclapTimer = 20000;
            }
        }
        else
            m_uiThunderclapTimer -= uiDiff;

        // Rage
        if (m_uiRageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RAGE) == CAST_OK)
                m_uiRageTimer = 35000;
        }
        else
            m_uiRageTimer -= uiDiff;

        if (m_uiPierceArmorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_PIERCEARMOR) == CAST_OK)
                m_uiPierceArmorTimer = 40000;
        }
        else
            m_uiPierceArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakkisath(Creature* pCreature)
{
    return new boss_drakkisathAI(pCreature);
}

void AddSC_boss_drakkisath()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_drakkisath";
    newscript->GetAI = &GetAI_boss_drakkisath;
    newscript->RegisterSelf();
}
