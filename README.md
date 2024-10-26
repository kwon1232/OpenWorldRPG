UE5 - RPG 게임 프로젝트
제작자 : 권지현

<프로젝트 전반적인 상속 구조>
 
![image](https://github.com/user-attachments/assets/ae70b350-10a5-4c5f-ab76-4905eb951149)
![image](https://github.com/user-attachments/assets/5f69f825-60e8-45b9-9c3b-3d305dfa376b)
 

<상속구조와 관여 없이 전방 선언과 전처리기를 이용하여 사용한 클래스>
 

![image](https://github.com/user-attachments/assets/0892934b-3212-44be-8813-353793b69c58)



RPG 프로젝트 내 아이템 기능

1.	골드 획득 및 소울 획득
 ![image](https://github.com/user-attachments/assets/26989cc9-a3d3-43ae-8050-72d8243b2849)


1-1.	골드 획득 및 소울 방법

플레이어가 몬스터를 죽이면 몬스터 특성에 맞는 보석을 드랍 하도록 설정

 ![image](https://github.com/user-attachments/assets/8ca5d490-47c6-451c-8b58-b48570831b44)



해당 골드 및 소울을 획득할 수 있도록 한 코드
<ASoul Class의 코드>  

![image](https://github.com/user-attachments/assets/4ed4e005-9f11-4d54-aad6-64cb283b972f)

<ATreasure Class의 코드>

 ![image](https://github.com/user-attachments/assets/c280b4c3-676d-4e89-8a42-aebbd2aa8df5)



2.	Weapon 부여 방법

   
2-1.	플레이어가 무기 들기

범용적으로 사용할 수 있는 eapon Class와 


플레이어가 무기와 관련된

주무기 장착, 보조무기 장착, 무기 미장착 상태를 판별할

Weapon과 관련된 Enum Class를 만들어준다.

Weapon Class에서는 C++ 부분에서 대략적인 수치 값을 설정해주고,

BP에서 BP_BaseWeapon을 생성 후

자식 BP를 생성하여 수치 값을 수정하여 사용하였다.

<Weapon 관련 코드 첨부>

 ![image](https://github.com/user-attachments/assets/39208ba1-1160-4fad-b101-265a58d7ee38)


<AItem의 Sphere를 코드 해당 기능을 상속해준다>

 ![image](https://github.com/user-attachments/assets/c2d6220f-90c3-4775-b91e-c24830a1d442)
![image](https://github.com/user-attachments/assets/39a7d5a9-7ce7-4d23-b64d-c5fd31e55613)

 

<BP_AItem Spher 예시>

 ![image](https://github.com/user-attachments/assets/59c90851-57cf-461a-afc3-0dbaeb11e9cc)

<BP_Weapon에서 상속받은 Spher 반경>
 ![image](https://github.com/user-attachments/assets/4d9cfdcb-d60f-4369-a8ac-58c3996d030b)

Sphere 콜리전을 overlapd으로 설정해준 뒤 반경 안에 들어왔음을 판별한다.

판별 방식은 Observer Pattern을 사용하였다.

OnSphereOverlap() 함수와 OnSphereEndOverlap() 함수를 이용하여 

Player에게 무기를 집어들 수 있는 반경 안에 들어왔음을 판별한다.

<Delegate 이용하여 Observer Pattern 제작>

 ![image](https://github.com/user-attachments/assets/7f8ff672-73d5-479a-8f4c-0ad98e7f9325)


인터페이스에 해당 무기의 주소를 넘겨주어,

플레이어 클래스에서는 포인터를 이용해 다양한 설정 값을 받아올 수 있다.

해당 범위를 벗어난다면 nullptr을 설정해주어 해제시킨다.

 ![image](https://github.com/user-attachments/assets/1b4fec22-9ae8-4ca1-a9e2-7cbc4fbe2d93)

2-2-2. 무기 상태에 따른 플레이어 모션 변화


C++에서 모션이 바뀌는 조건을 설정해준 뒤 AnimBP에서 해당 모션을 설정했다.  

![image](https://github.com/user-attachments/assets/2866d2e0-88ca-4a3d-bdf5-05e31b2eca8a)




3.	Riding System 구현


3-1. 애니메이션 블루 프린트에서 값 전달 실패 및 개선
  	
Riding System을 구현하기 위해서 블렌드 스페이스 1D 구현을 위하여,

탈 것에게 플레이어가 속도 값을 가져와야 하는 경우를 

블루 프린트로 먼저 구현하려 시도했다.

하지만 플레이어 AnimBP가 C++에서 값을 받아오는 구조이기 때문에

블루 프린트만으로는 구현되지 않았다.

때문에 다음과 같이 

라이더 컴포넌트에서 탈 것 컴포넌트 안에 있는 탈 것 캐릭터의 

이동 값을 다음과 같이 받아오며 해결하였다.

 ![image](https://github.com/user-attachments/assets/b849d8e7-786c-41ff-be52-221b118f1441)


3-2. 라이딩 시스템 모듈화로 코드의 재사용성 높이기
<Horse.h>
 ![image](https://github.com/user-attachments/assets/5f8a2313-b0c1-4468-81a4-282b23667b94)

<BaseCharacrer.h>
 ![image](https://github.com/user-attachments/assets/92a9c056-00b1-4892-a9c6-a758ecfb8e14)


라이딩 시스템 관련된 코드를 

컴포넌트로 나누어 모듈화를 구현하고자 노력하였다.

Rider Component에는 운전자의 주요 기능을 넣어주었으며,

Riding Component에는 탈 것의 주요 기능을 넣어주었다.

코드의 재사용성을 고려하여 해당 컴포넌트를 추가하기만 하면

RingSystem 구현이 가능하도록 하였다.


구현 내용

Rider Component에서 탈 수 있는지 없는지에 대한 변수 값을 가지고 있고,

R키를 누르면 반경 안에 있는 액터가 있는지,

액터가 있다면 탈 것이 맞는 지 판별한다.

캐릭터 앞의 구로 해당 영역 내에 있는 것이 

Actor인지, Actor라면 Character로 캐스팅한 뒤

Riding Component가 포함되어 있는지 판별하여 탈 것인지 판별한다. 


<해당 기능 Play 사진>

 ![image](https://github.com/user-attachments/assets/805dec05-ee0f-4608-bbd6-30ab5948f3c5)

Rider Componet의 Trace()함수 호출 후, 

만약 RidingSystem있는 캐릭터를 발견했으면 탈 것으로 판별한다.

그 후에는 Mount()라는 함수를 호출하여 다음과 같은 작업을 수행한다.

Riding System에서는 해당 값을 받아오고,

Rider Controller를 탈 것에게 권한을 허용한 뒤

라이더를 탈 것에게 부착한다.


 ![image](https://github.com/user-attachments/assets/3a313b0b-132a-4e99-9338-e65993416717)


이 외에도 세부적으로는 탈 것의 후진이 가능하도록 

GetCharacterMovement()->bOrientRotationToMovement값을 

false와 true로 조절하였으며,

ForwardWalkSpeed와 MaxWalkSpeed를 설정해 준 뒤,

ForwardWalkSpeed가 MaxWalkSpeed를 초과하지 않으면

가속이 가능하도록 제작하였다.



4.	벡터의 내적 및 외적의 사용


4-1. Hit 방향 판별을 위한 내적과 외적의 활용

Player와 Enemy 모두 공격을 하고 타격을 입기 때문에

Hit 방향에 맞게 밀려나는 현상을 구현하기 위해서, 

타격 지점을 계산하는 코드를 제작하였다.

<해당 코드>

 ![image](https://github.com/user-attachments/assets/e60cbf38-7787-4c46-a007-5528b1472c3e)

해당 방향을 구한 뒤에는 방향에 맞는 몽타주 섹션을 재생하였다. 

<Play 화면에서의 히트 방향 계산 가시화>

 ![image](https://github.com/user-attachments/assets/b28329f4-a087-44ef-9c36-1a7ffdd1632f)




5.	Physics Field System를 이용한 부서지는 액터와 무작위 아이템 드랍

5-1. 부서지는 액터의 콜리전 설정

부서지는 액터가 Player IK에 영향을 주지 않도록 

다음과 같은 콜리전을 코드를 구현하였다.

<BreakableActor.cpp>

 ![image](https://github.com/user-attachments/assets/b4e55481-5a61-47d8-b332-538b70f774ad)
![image](https://github.com/user-attachments/assets/a3bb5b19-e456-4554-b366-0c3bc618bce5)
  ![image](https://github.com/user-attachments/assets/5a495b31-c7df-4c52-afdd-734d57284c03)

![image](https://github.com/user-attachments/assets/9e3debc7-0fa4-4f0c-b1bd-7b54bfee3e4d)

 

위와 같은 콜리전 설정으로 플레이어에게는 캡슐로 무시되지만,

플레이어가 들고있는 무기에는 Block되도록 설정하였으며,

충돌이 일어난 뒤에는 콜리전 설정을 변경하고,

LifeTime을 설정하여 사라지도록 하였다.



5-2. 무작위 아이템 드랍


ATreature클래스를 TArray로 받아온 뒤,

해당 액터에 나올 보석을 BP에서 설정하여,

보석 배열의 인덱스에 보석이 None으로 설정되면 나오지 않고,

보석 종류를 설정해 놓으면 랜덤으로 

해당 인덱스의 보석이 드랍 되도록 구현하였다.

BP에서는 BP_BaseBreakableActor를 생성한 후,

자식 블루 프린트 클래스를 

생성하여 인덱스 설정을 기본 값으로 상속받도록 해주었다.

<ABreakableActor CPP코드와 BP Treature index> 


 ![image](https://github.com/user-attachments/assets/d3173fcd-5ece-4369-a31a-b7f9b9629614)
![image](https://github.com/user-attachments/assets/3071cfe5-12cb-4e8d-b10b-01ccce24cb3a)

 ![image](https://github.com/user-attachments/assets/7495347a-2113-4ac8-9b44-e86f9ad133e9)

 
