![Playgroud Logo](./img/playground.png)

---
  

![Simulation Demo](./img/simulation_demo1.gif)
![Simulation Demo](./img/simulation_demo2.gif)

  
**Playground** 는 C++ 로 개발된 물리 엔진입니다.  
3차원 강체 물리 시뮬레이션, 그래픽 렌더링, 사용자 상호작용을 지원합니다.  

## 지원 기능  
- **물리 시뮬레이션**  
  
  - 강체(구 & 직육면체) 시뮬레이션
  - 실시간 충돌 감지 (*Separating Axis Test*)
  - Sequential Impulse Solver 기반 충돌 처리  
  
- **그래픽**  
  
  - 구 & 직육면체 렌더
  - 카메라 줌 & 이동 & 회전(Arcball-rotation)
  
## 의존성
- [GLAD](https://glad.dav1d.de) (0.1.33 버전에서 작동 확인)
- [GLFW](https://www.glfw.org) (3.3 버전에서 작동 확인)
- [Dear ImGui](https://github.com/ocornut/imgui) (1.79 버전에서 작동 확인)
  
## 빌드하기  
아래의 명령어를 터미널에 입력합니다.  
```shell
g++ -o playground src/main.cpp src/playground/* src/physics/* src/graphics/* src/gui/* -std=c++11 -framework OpenGL -lglfw -I include -DIMGUI_IMPL_OPENGL_LOADER_GLAD
```  
  
  
>  
> [Wiki](https://github.com/JmirY/Playground/wiki) 를 읽고 Playground 에 대해 더 알아보세요.  
>   