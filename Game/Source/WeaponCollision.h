#pragma once
#include "CollisionComponent.h"
#include "HealthComponent.h"


class WeaponCollision : public CollisionComponent {
public:
    int damage = 1;
    float hitStop = 0.5f;
    bool isActive = false;

    void SetAttack(float Damage, float HitStop)
    {
        damage = Damage;
        hitStop = HitStop;
    }

    void OnCollision(std::shared_ptr<CollisionComponent> other) override {
        // 1. 相手が自分自身（Player）なら無視
// 攻撃判定が無効、または相手が自分なら無視
        if (!isActive || other->GetActor()->GetName() == std::string("Player")) return;

        auto health = other->GetActor()->GetComponent<HealthComponent>();
        if (health) {
            // 名前が Enemy の場合だけダメージ
            if (std::string(other->GetActor()->GetName()) == "Enemy") {
                health->ApplyDamage(damage, hitStop);

                // 1回当たったら無効化する（多重ヒット防止）
                isActive = false;
            }
        }
    }
};
