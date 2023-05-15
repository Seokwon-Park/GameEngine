# Primal Engine Project
Game Engine Programming Series By Arash Khatami   
https://www.youtube.com/@GameEngineSeries

![image](https://user-images.githubusercontent.com/22045739/233913997-e5423f5a-ecb2-4fd5-946a-2b236c4e3f38.png)

#### 2023-05-01   
define 매크로 작성 때 문법 문제로 원인 찾느라 시간 낭비 많이함.   
-> 처음 작성할 때 철저하게   

#### 2023-05-02   
LNK1104 cannot open file 'engine.lib'   
두 번 빌드하면 잘 됨 -> 원인?
Build Order 문제(engine.lib보다 enginedll이 먼저 build되서)  
Dependency 설정(유튜브 영상에도 나중에 설명나옴)

### 2023-05-05   
Geometry 데이터 저장 방식에 대해 추가 공부 필요

자주 발생한 오류 체크리스트   
- 오타 확인, 특히 셰이더 부분 "name.HLSL"
- initialize와 release 올바르게 했는지 체크 (순서 release는 initialize의 역순)
- header include 순서 체크
- pointer, reference 인자값 전달 체크
- nullptr 문제의 경우 return을 빼먹었는지 or &, *을 안썼는지
   
Issue   
SetGraphicsRoot32Constants로 shader에 Constant 전달이 안된다.......
이유는 모름..
