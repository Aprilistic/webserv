  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

// location 을 찾아갔는데 없으면 root로 가야하고 그것도 없으면 NULL 







<!-- //비동기적 파일 I/O (AIO)를 사용해서 파일을 읽기 전에 kqueue 이벤트 등록
//읽기 완료되면 kqueue에 알림이 옴
//response 만들어 보내기

//예시 코드
#include <aio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024

int main() {
    int kq = kqueue();  // kqueue 생성
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }

    struct aiocb aio;
    char buffer[BUFFER_SIZE];
    int fd = open("somefile.txt", O_RDONLY);
    if(fd == -1) {
        perror("open");
        return 1;
    }

    memset(&aio, 0, sizeof(aio));
    aio.aio_fildes = fd;
    aio.aio_buf = buffer;
    aio.aio_nbytes = BUFFER_SIZE;
    aio.aio_sigevent.sigev_notify = SIGEV_KEVENT;   // kqueue 알림 사용
    aio.aio_sigevent.sigev_notify_kqueue = kq;
    aio.aio_sigevent.sigev_value.sival_ptr = &aio;

    if (aio_read(&aio) == -1) {
        perror("aio_read");
        close(fd);
        return 1;
    }

    // kevent 구조체 초기화
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_AIO, EV_ADD, 0, 0, &aio);

    struct timespec timeout = { 10, 0 };  // 10초 타임아웃
    int ret = kevent(kq, &kev, 1, &kev, 1, &timeout);

    if (ret == -1) {
        perror("kevent");
    } else if (ret > 0) {
        int numBytes = aio_return(&aio);
        if(numBytes != -1) {
            buffer[numBytes] = '\0';
            printf("Read: %s\n", buffer);
        } else {
            perror("aio_return");
        }
    } else {
        printf("No data read within 10 seconds\n");
    }

    close(fd);
    close(kq);
    return 0;
} -->
