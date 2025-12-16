#pragma once
#include "JudgmentBase.h"
#include "Enemy.h"


// BattleNode‚É‘JˆÚ‚Å‚«‚é‚©”»’è
class BattleJudgment : public JudgmentBase
{
public:
	BattleJudgment(Enemy* enemy) :JudgmentBase(enemy) {};
	// ”»’è
	bool Judgment();
};

// œpœjs“®
class WanderJudgment : public JudgmentBase
{
public:
	WanderJudgment(Enemy* enemy) : JudgmentBase(enemy) {}
	// ”»’è
	bool Judgment();
};

// UŒ‚s“®
class AttackJudgment : public JudgmentBase
{
public:
	AttackJudgment(Enemy* enemy) : JudgmentBase(enemy) {}
	// ”»’è
	bool Judgment();
};

// ’ÇÕs“®
class PursuitJudgment : public JudgmentBase
{
public:
	PursuitJudgment(Enemy* enemy) : JudgmentBase(enemy) {}
	// ”»’è
	bool Judgment();
};

