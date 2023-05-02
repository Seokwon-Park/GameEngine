# Primal Engine Project
Game Engine Programming Series By Arash Khatami   
https://www.youtube.com/@GameEngineSeries

![image](https://user-images.githubusercontent.com/22045739/233913997-e5423f5a-ecb2-4fd5-946a-2b236c4e3f38.png)

#### 2023-05-01   
define 매크로 디버깅이 어렵기 때문에 처음 작성할 때 철저하게   

#### 2023-05-02   
LNK1104 cannot open file 'engine.lib'   
두 번 빌드하면 잘 실행된다 => Build Order issue   
engine.lib을 build 하기 전에 이 lib을 사용하는 다른 프로젝트를 build 해버려서 생기는 문제   
Solution 우클릭 Project Build Order.. 설정
