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
	//// ��Ŷ�� Queue�� �־��ش�.
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
		printf("��Ŷ ���� �̻� ����..!\n");
	}
	else {

		// ���� ���� ���� �����ֱ�.
		int retval = send(sock, (char *)&packet_size, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}

		// ���� ������ �����ֱ�
		retval = send(sock, reinterpret_cast<CHAR *>(buf), packet_size, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
	}

}

void SendPacketQueue() {
	// Queue�� ������ �Ǿ� ���� ��쿡�� ó���� �Ѵ�.
	if (packet_queue.empty()  == false) {
		// Packet_Queue ����
		Packet_Queue pq;
		// ���� �տ��ִ� Packet�� �����´�.
		pq = packet_queue.front();
		// Packet�� ���������� ������ Packet�� ����ش�.
		packet_queue.pop_front();


		// Type�� ���������� ������ Ȯ�� �ϱ� ���Ͽ� 
		if ((int)pq.buf[0] >= LIMIT_PACKET_SERVER_NO || !(pq.packet_size >= 1 && pq.packet_size<=5000)) {
			printf("��Ŷ ���� �̻� ����..!\n");
		}
		//else {
			// ���� ���� ���� �����ֱ�.
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