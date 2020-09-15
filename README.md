# Operating-System

#### 과제 #0
리눅스 VM 생성 및 커널 컴파일

<hr>

#### 과제 #1
커널 레벨 클립보드 서비스 - K-Board   

* 구현 내용
  * 커널 내에 ring buffer 기반으로 클립을 관리하는 K-board 구현
  * K-board에 접근하기 위한 시스템 콜 구현: enqueue, dequeue
  * K-board를 사용하는 예제 프로그램 구현: copy, paste

<hr>

#### 과제 #2
K-Board v2: over ProcFS   

* 구현 내용
  * os_kboard.c 에서 구현한 내용을 모듈로 이전
    * 예제 프로그램을 참고하여, 필요한 부분 수정, 확장하여 진행
  * Enqueue, dequeue 인터페이스를 Proc FS의 write(), read()로 변경
    * /proc/kboard/writer
  * Queue 내용에 대해 Read 만 수행할 수 있는 인터페이스 제공
    * /proc/kboard/reader
  * Count 값과 버퍼 상태를 볼 수 있는 인터페이스도 구현
    * /proc/kboard/count, /proc/kboard/dump
    

* **Proc File System**
  * /proc
    * 커널-유저 간 편리한 정보 전달을 위한 특수 파일 시스템
      * 파일 시스템 인터페이스를 이용해 커널 공간의 데이터를 접근
      * Open(), close(), read(), write(), …
      * struct file_operations 에 정의되어 있음
    * Pseudo file system
      * 일반 파일 시스템: 스토리지에 저장된 파일의 데이터에 접근
      * Pseudo file system: read(), write()는 편리한 인터페이스일 뿐, 어떤 동작을 하든 자유
        * Proc의 경우, 주로 커널의 데이터를 읽거나, 변경하는데 이용됨
        * CPU, 메모리 정보: /proc/cpuinfo, /proc/meminfo
        * 커널에 명령을 전달: echo 3 > /proc/sys/vm/drop_caches
        
  * read(), write()와 같은 기존에 정의된 시스템콜 활용
