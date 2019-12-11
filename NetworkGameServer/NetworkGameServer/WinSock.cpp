#include "main.h"
#include "protocol.h"

std::deque<Packet_Queue> packet_queue;

void err_quit(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char *buf, int len, int flags) {
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void SendPacket(SOCKET sock, void *packet, int packet_size) {
	//// 패킷을 Queue에 넣어준다.
	//Packet_Queue pq;
	////pq.packet = packet;
	//pq.packet_size = packet_size;
	//printf("%d\t", pq.packet_size);
	//memcpy(pq.buf, packet, pq.packet_size);
	//pq.buf[pq.packet_size] = '\0';
	//pq.sock = sock;
	//packet_queue.emplace_back(pq);


	/*char buf[MAX_BUFSIZE];*/
	unsigned char buf[MAX_BUFSIZE];
	memcpy(buf, packet, packet_size);
	//buf[packet_size] = '\0';

	if ((int)buf[0] >= LIMIT_PACKET_SERVER_NO) {
		printf("패킷 전송 이상 오류..!\n");
	}
	else {

		// 고정 길이 먼저 보내주기.
		int retval = send(sock, (char *)&packet_size, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		// 실제 데이터 보내주기
		retval = send(sock, reinterpret_cast<CHAR *>(buf), packet_size, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
	}

}

void SendPacketQueue() {
	// Queue에 삽입이 되어 있을 경우에만 처리를 한다.
	if (packet_queue.empty()  == false) {
		// Packet_Queue 생성
		Packet_Queue pq;
		// 가장 앞에있는 Packet을 가져온다.
		pq = packet_queue.front();
		// Packet을 가져왔으니 가져온 Packet을 비워준다.
		packet_queue.pop_front();


		// Type이 정상적으로 들어가는지 확인 하기 위하여 
		if ((int)pq.buf[0] >= LIMIT_PACKET_SERVER_NO || !(pq.packet_size >= 1 && pq.packet_size<=5000)) {
			printf("패킷 전송 이상 오류..!\n");
		}
		//else {
			// 고정 길이 먼저 보내주기.
		int retval = send(pq.sock, (char *)&pq.packet_size, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		retval = send(pq.sock, pq.buf, pq.packet_size, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
	}
	//}

}