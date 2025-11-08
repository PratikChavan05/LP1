import socket
import threading


HOST ="127.0.0.1"
PORT=3000

def client_send(socket_id,msg):
    client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    client.connect((HOST,PORT))
    print(f"connected to server")
    client.sendall(msg.encode())
    data=client.recv(1024)
    print(f"msg :",{data.decode()})
    client.close()


def main():
    messages=[
        "1",
        "2",
        "3"
    ]


    for i, msg in enumerate(messages,start=1):

        thread = threading.Thread(target=client_send,args=(i,msg))
        thread.start()

if __name__ == "__main__":
    main()