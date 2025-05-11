UE5 - RPG 게임 프로젝트
제작자 : 권지현

```mermaid
graph TD
    A[Tick() - AI StateCheck] --> B{IsDead?}
    B -- Yes --> Z[Return]
    B -- No --> C{EnemyState > Patrolling?}
    C -- Yes --> D[CheckCombatTarget()]
    C -- No --> E[CheckPatrolTarget()]

    D --> F{IsOutsideCombatRadius?}
    F -- Yes --> G[LoseInterest()]
    G --> H{IsEngaged?}
    H -- No --> I[StartPatrolling()]

    F -- No --> J{IsOutsideAttackRadius AND !IsChasing?}
    J -- Yes --> K[ChaseTarget()]
    J -- No --> L{CanAttack()?}
    L -- Yes --> M[StartAttackTimer()]
    L -- No --> N[Do Nothing]

    E --> O{InTargetRange(PatrolTarget)?}
    O -- Yes --> P[ChoosePatrolTarget() + SetTimer]
    O -- No --> Q[Continue Moving]

    style Z fill:#fdd
    style M fill:#f9f,stroke:#900,stroke-width:2px
    style K fill:#bbf
    style I fill:#bfb

```
