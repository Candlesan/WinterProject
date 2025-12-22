#pragma once
#include "CollisionComponent.h"
#include "HealthComponent.h"


class WeaponCollision : public CollisionComponent {
public:
    void OnCollision(std::shared_ptr<CollisionComponent> other) override {
        // 1. 相手が自分自身（Player）なら無視
        if (other->GetActor()->GetName() == std::string("Player")) return;

        // 2. 相手のアクターに HealthComponent が付いているかチェック
        // EnemyはCollisionComponentを継承していなくても、アクターとしてHealthを持っていればOK
        auto health = other->GetActor()->GetComponent<HealthComponent>();
        if (health) {
            // 当たった相手が Enemy であることを確認してダメージ
            if (std::string(other->GetActor()->GetName()) == "Enemy") {
                health->ApplyDamage(1, 0.5f);
            }
        }
    }
};
