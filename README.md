![Playgroud Logo](./img/playground.png)

---
  

![Simulation Demo](./img/simulation_demo.gif)

  
**Playground** 는 키보드와 마우스를 통해 조작 가능한 강체 물리 엔진입니다.  
아래와 같은 기능들을 지원합니다.  
- Semi-implicit Euler 적분
- 충돌 감지 (*Separating Axis Test*)
- 충격량 (*Impulse*) 기반 충돌 처리
- GUI 를 통한 시뮬레이션 조작  
  
## 빌드하기  
---
아래의 명령어를 터미널에 입력합니다.  
```shell
g++ -o playground src/main.cpp src/playground/* src/physics/* src/graphics/* src/gui/* -std=c++11 -framework OpenGL -lglfw -I include DIMGUI_IMPL_OPENGL_LOADER_GLAD
```
