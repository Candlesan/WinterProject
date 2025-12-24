#pragma once
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include <string>


class WeaponCollision : public CollisionComponent {
public:
    int damage = 1;
    float hitStop = 0.5f;
    bool isActive = false;

    std::string ownerName = "Player";

    void SetAttack(float Damage, float HitStop)
    {
        damage = Damage;
        hitStop = HitStop;
    }

    void OnCollision(std::shared_ptr<CollisionComponent> other) override
    {
        // 攻撃判定が無効、または相手が自分なら無視
        if (!isActive || other->GetActor()->GetName() == ownerName) return;

        auto health = other->GetActor()->GetComponent<HealthComponent>();
        if (health) 
        {
            health->ApplyDamage(damage, hitStop);

            // 1回当たったら無効化する（多重ヒット防止）
            isActive = false;
        }
    }
};
