#pragma once
#include "ActionBase.h"
#include "Enemy.h"


// ë“ã@çsìÆ
class IdleAction : public ActionBase
{
public:
	IdleAction(Enemy* enemy) : ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// úpújçsìÆ
class WanderAction : public ActionBase
{
public:
	WanderAction(Enemy* enemy) : ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// çUåÇçsìÆ
class AttackAction : public ActionBase
{
public:
	AttackAction(Enemy* enemy) : ActionBase(enemy) {}
	ActionBase::State Run(float elapsedTime);
};

// í«ê’çsìÆ
class PursuitAction : public ActionBase
{
public:
	PursuitAction(Enemy* enemy) : ActionBase(enemy){}
	ActionBase::State Run(float elapsedTime);
};
