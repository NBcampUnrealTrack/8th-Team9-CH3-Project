// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/PoliceZombie_ai.h"


// 본 코드에선 좀비의 AI를 다룹니다. 플레이어 TAG에 "Player" 를 넣어주시면 좀비가 감지해서 반응합니다.

APoliceZombie_ai::APoliceZombie_ai()
{
    //  시야 상세 설정: 여기서 수치를 바꾸면 AI의 탐지 능력이 변합니다.
    if (SightConfig) {
        SightConfig->SightRadius = 1500.0f;           // 인식 거리 (15미터
        SightConfig->LoseSightRadius = 2000.0f;       // 놓치는 거리 (시야에서 멀어져도 20미터까지는 추격)
        SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 시야각 (정면 기준 양옆 60도, 총 120도 범위를 봄)
        SightConfig->SetMaxAge(5.0f);                 // 마지막으로 본 위치를 5초간 기억함
    }
}
